#include "esportazione_obj.h"
#include "gestione_multicurva.h"
#include "gestione_curve.h"
#include "earcut.hpp"
#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdint>
#include <algorithm>

// Campionamento CPU: non dipende dal fatto che il rendering sia gia avvenuto.
extern int param; // 0: uniforme; 1: lunghezza delle corde
static bool CampionaPerOBJ(Curva& c, std::vector<glm::vec3>& out) {
    const size_t n = c.CP.size();
    if (!c.chiusaC1 || n < 4 || c.Derivata.size() < n ||
        c.Tens.size() < n || c.Bias.size() < n || c.Cont.size() < n) return false;
    std::vector<float> knots(n, 0.f);
    for (size_t i = 1; i < n; ++i) {
        float step = param == 0 ? 1.f : glm::distance(c.CP[i], c.CP[i-1]);
        if (step < 1.e-6f) return false;
        knots[i] = knots[i-1] + step;
    }
    if (knots.back() <= 0.f) return false;
    for (float& v : knots) v /= knots.back();
    AggiornaChiusuraC1(&c, knots.data());
    const int samplesPerSegment = 32;
    out.clear();
    for (size_t i = 0; i + 1 < n; ++i) {
        const float h = knots[i+1] - knots[i];
        const float x0 = DX(static_cast<int>(i), knots.data(), &c);
        const float y0 = DY(static_cast<int>(i), knots.data(), &c);
        const float x1 = DX(static_cast<int>(i+1), knots.data(), &c);
        const float y1 = DY(static_cast<int>(i+1), knots.data(), &c);
        for (int j = 0; j < samplesPerSegment; ++j) {
            const float u = float(j) / float(samplesPerSegment);
            const float u2 = u*u, u3 = u2*u;
            const float a = 2*u3 - 3*u2 + 1;
            const float b = u3 - 2*u2 + u;
            const float d = -2*u3 + 3*u2;
            const float e = u3 - u2;
            out.emplace_back(a*c.CP[i].x + b*h*x0 + d*c.CP[i+1].x + e*h*x1,
                             a*c.CP[i].y + b*h*y0 + d*c.CP[i+1].y + e*h*y1, 0.f);
        }
    }
    return out.size() >= 3;
}

bool EsportaFormaOBJ(const std::string& percorso, std::string& messaggio) {
    const std::string::size_type slash = percorso.find_last_of("/\\");
    const std::string::size_type dot = percorso.find_last_of('.');
    const std::string base = (dot != std::string::npos && (slash == std::string::npos || dot > slash)) ? percorso.substr(0, dot) : percorso;
    const std::string mtlPath = base + ".mtl";
    const std::string mtlName = mtlPath.substr(mtlPath.find_last_of("/\\") == std::string::npos ? 0 : mtlPath.find_last_of("/\\") + 1);
    // Prepara prima la geometria: non creare file vuoti in caso di errore.
    struct Parte { size_t indice; std::vector<glm::vec3> punti; std::vector<uint32_t> triangoli; };
    std::vector<Parte> parti;
    size_t ignorate = 0;
    for (size_t k = 0; k < curve.size(); ++k) {
        if (!curve[k]) { ++ignorate; continue; }
        Parte parte; parte.indice = k;
        if (!CampionaPerOBJ(*curve[k], parte.punti)) { ++ignorate; continue; }
        using Point = std::array<double, 2>;
        std::vector<Point> ring;
        for (const auto& v : parte.punti) ring.push_back({double(v.x), double(v.y)});
        parte.triangoli = mapbox::earcut<uint32_t>(std::vector<std::vector<Point>>(1, ring));
        std::vector<uint32_t> validi;
        for (size_t j = 0; j + 2 < parte.triangoli.size(); j += 3) {
            const auto a = parte.triangoli[j], b = parte.triangoli[j+1], d = parte.triangoli[j+2];
            if (a >= parte.punti.size() || b >= parte.punti.size() || d >= parte.punti.size()) continue;
            const auto& A = parte.punti[a]; const auto& B = parte.punti[b]; const auto& C = parte.punti[d];
            const double area = (B.x-A.x)*(C.y-A.y)-(B.y-A.y)*(C.x-A.x);
            if (std::abs(area) <= 1.e-10) continue;
            if (area > 0) { validi.push_back(a); validi.push_back(b); validi.push_back(d); }
            else { validi.push_back(a); validi.push_back(d); validi.push_back(b); }
        }
        parte.triangoli.swap(validi);
        if (parte.triangoli.empty()) { ++ignorate; continue; }
        parti.push_back(std::move(parte));
    }
    if (parti.empty()) {
        messaggio = "Nessun triangolo esportabile: chiudere le curve (C1), verificare punti distinti e area non nulla. Nessun file OBJ vuoto creato.";
        return false;
    }
    std::ofstream obj(percorso.c_str());
    std::ofstream mtl(mtlPath.c_str());
    if (!obj || !mtl) { messaggio = "Impossibile creare OBJ/MTL: controllare il percorso."; return false; }
    obj << std::setprecision(9) << "# Forma Hermite esportata come triangoli\nmtllib " << mtlName << "\n";
    mtl << std::setprecision(9) << "# Materiali delle curve Hermite\n";
    size_t offset = 1, totaleVertici = 0, totaleTriangoli = 0;
    for (const auto& parte : parti) {
        const Curva& c = *curve[parte.indice];
        const std::string name = "Curva_" + std::to_string(parte.indice + 1);
        mtl << "newmtl " << name << "\nKa 0 0 0\nKd "
            << std::max(0.f, std::min(1.f, c.coloreCurva.r)) << ' '
            << std::max(0.f, std::min(1.f, c.coloreCurva.g)) << ' '
            << std::max(0.f, std::min(1.f, c.coloreCurva.b))
            << "\nKs 0 0 0\nd 1\nillum 1\n\n";
        obj << "\no " << name << "\ng " << name << "\nusemtl " << name << "\n";
        for (const auto& v : parte.punti) obj << "v " << v.x << ' ' << v.y << ' ' << v.z << '\n';
        for (size_t j = 0; j < parte.triangoli.size(); j += 3)
            obj << "f " << offset + parte.triangoli[j] << ' '
                << offset + parte.triangoli[j+1] << ' '
                << offset + parte.triangoli[j+2] << '\n';
        offset += parte.punti.size();
        totaleVertici += parte.punti.size();
        totaleTriangoli += parte.triangoli.size()/3;
    }
    obj.close(); mtl.close();
    if (!obj || !mtl) { messaggio = "Errore durante la scrittura dei file."; return false; }
    messaggio = "Esportate " + std::to_string(parti.size()) + " curve, " + std::to_string(totaleVertici) + " vertici e " + std::to_string(totaleTriangoli) + " triangoli in OBJ/MTL";
    if (ignorate) messaggio += "; ignorate " + std::to_string(ignorate) + " curve aperte/vuote/degeneri";
    return true;
}
