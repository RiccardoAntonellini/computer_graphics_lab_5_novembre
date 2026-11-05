#pragma once
#include "lib.h"
#include "strutture.h"
#include "inizializzazioni.h"
#include "gestione_curve.h"
#include "gestione_multicurva.h"
#include "gestione_interazioni.h"
#include "imgui.h"
#include <cmath>           // Libreria core ImGui
#include "imgui_impl_glfw.h" // Integrazione ImGui con GLFW (gestione finestra/input)
#include "imgui_impl_opengl3.h" // Integrazione ImGui con OpenGL3 (rendering)
extern GLFWwindow* window;
extern bool isMouseDown;
extern float r, g, b;
extern double mousex,mousey;
extern int height, width;
extern float* t;
extern bool showMenu;
extern Curva* curva;
extern int Mod;
extern bool modTg;
extern int selected_point;
 
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    mousex = xpos;
    mousey = height - ypos;
    // Il trascinamento viene gestito con polling per evitare conflitti con ImGui.
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // La selezione e il trascinamento sono gestiti da GestisciTrascinamentoCurve.
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        Mod = 0;
        modTg = false;
        selected_point = -1;
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // Se il tasto ESCAPE è premuto, chiude la finestra
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS)
            //Imposta a True il flag booleano di chiusura della finestr
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
    case GLFW_KEY_F1:
        if (action == GLFW_PRESS)
        {
            showMenu = !showMenu;
            if (!showMenu) CancellaAreePannelli();
        }
        break;
    case GLFW_KEY_T:
        if (mods & GLFW_MOD_SHIFT)
            curva->Tens[selected_point] = curva->Tens[selected_point] + 0.1;
        else
            curva->Tens[selected_point] = curva->Tens[selected_point] - 0.1;
        //Aggiorno la derivata nel punto selezionato utilizzando
        // Tens,Bias e Cont aggiornati per il vertice selezionato
        curva->Derivata.at(selected_point).x = dx(selected_point, t, curva);
        curva->Derivata.at(selected_point).y = dy(selected_point, t, curva);

        break;

    case GLFW_KEY_C:
        if (mods & GLFW_MOD_SHIFT)
            curva->Cont[selected_point] = curva->Cont[selected_point] + 0.1;
        else
            curva->Cont[selected_point] = curva->Cont[selected_point] - 0.1;
        //Aggiorno la derivata nel punto selezionato utilizzando
        // Tens,Bias e Cont aggiornati per il verice selezionato
        curva->Derivata.at(selected_point).x = dx(selected_point, t, curva);
        curva->Derivata.at(selected_point).y = dy(selected_point, t, curva);

        break;

    case GLFW_KEY_B:
        if (mods & GLFW_MOD_SHIFT)
            curva->Bias[selected_point] = curva->Bias[selected_point] + 0.1;
        else
            curva->Bias[selected_point] = curva->Bias[selected_point] - 0.1;

        //Aggiorno la derivata nel punto selezionato utilizzando
        // Tens,Bias e Cont aggiornati per il verice selezionato
        curva->Derivata.at(selected_point).x = dx(selected_point, t, curva);
        curva->Derivata.at(selected_point).y = dy(selected_point, t, curva);

        break;
   
    default:

        break;
    }

}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
    glViewport(0.0, 0.0, w, h);
   
}

// Le finestre ImGui possono catturare il mouse anche fuori dai loro rettangoli.
// Per l'editor, escludiamo soltanto i rettangoli effettivamente occupati dai pannelli.
namespace {
struct AreaPannello { float x=0, y=0, w=0, h=0; };
AreaPannello areePannelli[2];
bool MouseSuPannello(double x, double y) {
    if (!showMenu) return false; // F1: GUI nascosta, nessuna area riservata.
    for (const auto& a : areePannelli)
        if (a.w > 0 && a.h > 0 && x >= a.x && x <= a.x+a.w && y >= a.y && y <= a.y+a.h)
            return true;
    return false;
}
}
void CancellaAreePannelli() {
    for (auto& a : areePannelli) a = {};
}
void RegistraAreaPannello(float x, float y, float larghezza, float altezza, int indice) {
    if (indice < 0 || indice >= 2) return;
    areePannelli[indice] = {x, y, larghezza, altezza};
}

// Inserimento gestito per polling: indipendente dalla catena delle callback GLFW/ImGui.
void GestisciInserimentoCurve(GLFWwindow* window) {
    static bool precedentePremuto = false;
    const bool premuto = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    const bool nuovoClick = premuto && !precedentePremuto;
    precedentePremuto = premuto;
    if (!nuovoClick) return;
    if (Mod != 0) { std::cout << "[Multicurva] Click ignorato: modalita " << Mod << " (richiesta 0)\n"; return; }
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (MouseSuPannello(xpos, ypos)) {
        std::cout << "[Multicurva] Click su pannello ImGui, ignorato\n";
        return;
    }
    if (curvaAttivaIndice < 0 || curvaAttivaIndice >= static_cast<int>(curve.size())) return;
    curva = curve[curvaAttivaIndice].get();
    if (!curva || curva->chiusaC1) { std::cout << "[Multicurva] Curva assente o gia chiusa\n"; return; }
    int w = 0, h = 0;
    glfwGetWindowSize(window, &w, &h);
    if (w <= 0 || h <= 0) return;
    const float x = 2.0f * static_cast<float>(xpos) / w - 1.0f;
    const float y = 1.0f - 2.0f * static_cast<float>(ypos) / h;
    curva->CP.push_back(vec3(x, y, 0.0f));
    curva->colCP.push_back(vec4(0.0f, 1.0f, 1.0f, 1.0f));
    curva->Tens.push_back(0.0f);
    curva->Bias.push_back(0.0f);
    curva->Cont.push_back(0.0f);
    curva->Derivata.push_back(vec3(0.0f));
    curva->ncp = static_cast<int>(curva->CP.size());
    UPDATE_VAO_DYNAMIC(curva, true);
    std::cout << "[Multicurva] Punto inserito nella curva " << curvaAttivaIndice + 1
              << ", totale punti: " << curva->CP.size() << std::endl;
}

// Modifica dei punti di controllo e delle tangenti nella curva attiva.
// Nessuna modifica alle coordinate viene effettuata quando il mouse e' sopra ImGui.
void GestisciTrascinamentoCurve(GLFWwindow* window) {
    static bool premutoPrima = false;
    static bool trascina = false;
    static int punto = -1;
    static int estremoTangente = 0; // 0 = punto, +1/-1 = estremo tangente
    static int indiceCurvaDrag = -1;
    const bool premuto = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (!premuto) {
        premutoPrima = false;
        trascina = false;
        punto = -1;
        return;
    }
    if (Mod != 1 || curvaAttivaIndice < 0 || curvaAttivaIndice >= static_cast<int>(curve.size())) {
        trascina = false;
        premutoPrima = premuto;
        return;
    }
    Curva* c = curve[curvaAttivaIndice].get();
    if (!c || c->CP.empty()) { premutoPrima = premuto; return; }
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    if (w <= 0 || h <= 0) { premutoPrima = premuto; return; }
    const vec2 pos(2.0f * static_cast<float>(mx) / w - 1.0f,
                   1.0f - 2.0f * static_cast<float>(my) / h);
    if (!premutoPrima) {
        trascina = false;
        punto = -1;
        if (MouseSuPannello(mx, my)) { premutoPrima = true; return; }
        float migliore = 0.10f;
        estremoTangente = 0;
        for (int i = 0; i < static_cast<int>(c->CP.size()); ++i) {
            const vec2 centro(c->CP[i].x, c->CP[i].y);
            const float d = glm::distance(pos, centro);
            if (d < migliore) { migliore = d; punto = i; estremoTangente = 0; }
            if (modTg && c->CP.size() > 2) {
                // Le tangenti visualizzate sono CP +/- derivata/50.
                const vec2 deriv(DX(i, t, c), DY(i, t, c));
                for (int segno : {-1, 1}) {
                    const vec2 estremo = centro + (static_cast<float>(segno) / 50.0f) * deriv;
                    const float de = glm::distance(pos, estremo);
                    if (de < migliore) { migliore = de; punto = i; estremoTangente = segno; }
                }
            }
        }
        selected_point = punto;
        // In modalita' Modifica Tangenti il click sul vertice serve
        // esclusivamente a selezionarlo: non abilita lo spostamento.
        // Il drag e' consentito soltanto sugli estremi delle tangenti.
        trascina = punto >= 0 && (!modTg || estremoTangente != 0);
        indiceCurvaDrag = curvaAttivaIndice;
        premutoPrima = true;
        return;
    }
    if (!trascina || indiceCurvaDrag != curvaAttivaIndice || punto < 0 ||
        punto >= static_cast<int>(c->CP.size())) return;
    if (modTg && estremoTangente == 0) return;
    if (!modTg && estremoTangente == 0) {
        c->CP[punto] = vec3(pos.x, pos.y, 0.0f);
        if (c->chiusaC1 && (punto == 0 || punto == static_cast<int>(c->CP.size()) - 1)) {
            c->CP.front() = c->CP.back() = c->CP[punto];
        }
    } else {
        const vec2 centro(c->CP[punto].x, c->CP[punto].y);
        vec2 deriv = (pos - centro) * (50.0f * static_cast<float>(estremoTangente));
        // DX/DY usano zero come sentinella per il calcolo automatico.
        // Un epsilon mantiene rappresentabile anche una componente quasi nulla.
        if (std::abs(deriv.x) < 1e-6f) deriv.x = 1e-6f;
        if (std::abs(deriv.y) < 1e-6f) deriv.y = 1e-6f;
        c->Derivata[punto] = vec3(deriv.x, deriv.y, 0.0f);
        if (c->chiusaC1 && (punto == 0 || punto == static_cast<int>(c->CP.size()) - 1)) {
            c->Derivata.front() = c->Derivata.back() = c->Derivata[punto];
        }
    }
    UPDATE_VAO_DYNAMIC(c, true);
}
