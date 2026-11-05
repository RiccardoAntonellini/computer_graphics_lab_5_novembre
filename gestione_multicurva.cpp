#include "gestione_multicurva.h"
#include "inizializzazioni.h"
#include "gestione_curve.h"
#include <algorithm>
static const vec4 palette[] = {
    vec4(0.90f, 0.35f, 0.35f, 1.0f),
    vec4(0.20f, 0.70f, 0.90f, 1.0f),
    vec4(0.30f, 0.80f, 0.48f, 1.0f),
    vec4(0.95f, 0.70f, 0.22f, 1.0f),
    vec4(0.72f, 0.46f, 0.91f, 1.0f),
    vec4(0.96f, 0.46f, 0.70f, 1.0f),
    vec4(0.27f, 0.79f, 0.76f, 1.0f),
    vec4(0.85f, 0.80f, 0.35f, 1.0f),
};
std::vector<std::unique_ptr<Curva>> curve;
int curvaAttivaIndice = 0;
Curva* curva = nullptr;
void CambiaNumeroCurve(int numero) {
    numero = std::max(1, std::min(numero, 20));
    while (static_cast<int>(curve.size()) < numero) {
        std::unique_ptr<Curva> c(new Curva{});
        c->coloreCurva = palette[curve.size() % (sizeof(palette) / sizeof(palette[0]))];
        c->ncp = 100; c->nv = 200; c->ntg = 100;
        INIT_VAO_DYNAMIC(c.get(), true);
        curve.push_back(std::move(c));
    }
    // Non eliminiamo curve gia' costruite accidentalmente.
    if (curvaAttivaIndice >= static_cast<int>(curve.size())) curvaAttivaIndice = 0;
    curva = curve[curvaAttivaIndice].get();
}
void InizializzaCurve(int numero) { CambiaNumeroCurve(numero); }
void SelezionaCurva(int indice) {
    if (indice < 0 || indice >= static_cast<int>(curve.size())) return;
    curvaAttivaIndice = indice;
    curva = curve[indice].get();
    // La selezione dei punti viene azzerata dalla GUI.
}


#include <fstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <sstream>

namespace {
struct PuntoSalvato { float x,y,z,t,b,c; };
struct CurvaSalvata { bool chiusa; vec4 colore; std::vector<PuntoSalvato> punti; };
}

bool SalvaForma(const std::string& percorso, std::string& errore) {
    std::ofstream out(percorso.c_str(), std::ios::out | std::ios::trunc);
    if (!out) { errore = "Impossibile creare il file: " + percorso; return false; }
    out << "HERMITE_MULTICURVA 2\n" << curve.size() << " " << curvaAttivaIndice << "\n";
    out << std::setprecision(std::numeric_limits<float>::max_digits10);
    for (const auto& ptr : curve) {
        const Curva& c = *ptr;
        const size_t n = c.CP.size();
        if (c.Tens.size() != n || c.Bias.size() != n || c.Cont.size() != n) {
            errore = "Dati incoerenti: punti e parametri hanno dimensioni diverse."; return false;
        }
        out << "CURVA " << (c.chiusaC1 ? 1 : 0) << " " << n << " " << c.coloreCurva.r << " " << c.coloreCurva.g << " " << c.coloreCurva.b << "\n";
        for (size_t i = 0; i < n; ++i)
            out << c.CP[i].x << " " << c.CP[i].y << " " << c.CP[i].z << " "
                << c.Tens[i] << " " << c.Bias[i] << " " << c.Cont[i] << "\n";
    }
    out.flush();
    if (!out) { errore = "Errore durante la scrittura di " + percorso; return false; }
    errore.clear(); return true;
}

bool ApriForma(const std::string& percorso, std::string& errore) {
    std::ifstream in(percorso.c_str());
    if (!in) { errore = "File non trovato: " + percorso; return false; }
    std::string magic, tag;
    int versione = 0, numero = 0, attiva = 0;
    if (!(in >> magic >> versione) || magic != "HERMITE_MULTICURVA" || (versione != 1 && versione != 2) ||
        !(in >> numero >> attiva) || numero < 1 || numero > 20 || attiva < 0 || attiva >= numero) {
        errore = "Intestazione del file forma non valida."; return false;
    }
    std::vector<CurvaSalvata> dati;
    for (int k = 0; k < numero; ++k) {
        int chiusa = 0; long long n = 0;
        if (!(in >> tag >> chiusa >> n) || tag != "CURVA" || (chiusa != 0 && chiusa != 1) ||
            n < 0 || n > 100000 || (chiusa && n < 4)) {
            errore = "Intestazione non valida per la curva " + std::to_string(k + 1); return false;
        }
        CurvaSalvata c; c.chiusa = (chiusa == 1);
        c.colore = palette[k % (sizeof(palette) / sizeof(palette[0]))];
        if (versione == 2) {
            if (!(in >> c.colore.r >> c.colore.g >> c.colore.b) ||
                !std::isfinite(c.colore.r) || !std::isfinite(c.colore.g) || !std::isfinite(c.colore.b) ||
                c.colore.r < 0.f || c.colore.r > 1.f || c.colore.g < 0.f || c.colore.g > 1.f ||
                c.colore.b < 0.f || c.colore.b > 1.f) {
                errore = "Colore non valido nella curva " + std::to_string(k + 1); return false;
            }
        }
        for (long long i = 0; i < n; ++i) {
            PuntoSalvato p;
            if (!(in >> p.x >> p.y >> p.z >> p.t >> p.b >> p.c) ||
                !std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z) ||
                !std::isfinite(p.t) || !std::isfinite(p.b) || !std::isfinite(p.c)) {
                errore = "Punto non valido nella curva " + std::to_string(k + 1); return false;
            }
            c.punti.push_back(p);
        }
        if (c.chiusa) {
            const PuntoSalvato& a = c.punti.front();
            const PuntoSalvato& b = c.punti.back();
            if (std::abs(a.x-b.x)>1.e-4f || std::abs(a.y-b.y)>1.e-4f || std::abs(a.z-b.z)>1.e-4f) {
                errore = "Curva chiusa con estremi non coincidenti."; return false;
            }
        }
        dati.push_back(std::move(c));
    }
    std::string extra;
    if (in >> extra) { errore = "Dati aggiuntivi inattesi nel file."; return false; }
    // Applichiamo i dati solo dopo aver verificato l'intero file.
    CambiaNumeroCurve(numero);
    for (int k = 0; k < numero; ++k) {
        Curva& c = *curve[k];
        c.CP.clear(); c.colCP.clear(); c.Tens.clear(); c.Bias.clear();
        c.Cont.clear(); c.Derivata.clear(); c.tg.clear(); c.col_tg.clear();
        c.chiusaC1 = dati[k].chiusa;
        c.coloreCurva = dati[k].colore;
        for (const PuntoSalvato& p : dati[k].punti) {
            c.CP.push_back(vec3(p.x,p.y,p.z));
            c.colCP.push_back(vec4(0.f,0.f,0.f,1.f));
            c.Tens.push_back(p.t); c.Bias.push_back(p.b); c.Cont.push_back(p.c);
            c.Derivata.push_back(vec3(0.f,0.f,0.f));
        }
        c.ncp = static_cast<int>(c.CP.size());
        UPDATE_VAO_DYNAMIC(&c, true);
    }
    // Il numero effettivo e' gestito senza cancellare curve dalla funzione originale.
    // In fase di caricamento rimuoviamo esplicitamente quelle in eccesso.
    if (static_cast<int>(curve.size()) > numero) curve.resize(numero);
    SelezionaCurva(attiva);
    errore.clear(); return true;
}
