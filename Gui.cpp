#include <cmath>
#pragma once
// Garantisce che il file venga incluso una sola volta in fase di compilazione
#include <fstream>
#include <string>
#include <cstdio>
#include "strutture.h"
#include "imgui.h"           // Libreria core ImGui
#include "imgui_impl_glfw.h" // Integrazione ImGui con GLFW (gestione finestra/input)
#include "imgui_impl_opengl3.h" // Integrazione ImGui con OpenGL3 (rendering)
#include "lib.h"
#include "gestione_curve.h"
#include "gestione_multicurva.h"
#include "gestione_interazioni.h"
#include "esportazione_obj.h"
#include "inizializzazioni.h"
#include <math.h> // Libreria matematica standard C
#pragma warning(disable:4996)

extern ImVec4 clear_color;  // Colore di sfondo (RGBA), dichiarato esternamente
extern double mousex, mousey; // Coordinate del mouse, dichiarate esternamente
extern bool flag_wireframe;
extern int width, height;
extern int Mod, selected_point;
extern bool visualizzaTg, modTg;
extern bool riempi, visualizza_PC;
extern Curva* curva;
extern float* t;
extern float Tens, Bias, Cont;
extern int param;
 
// Funzione che inizializza ImGui con GLFW e OpenGL
void Initialize_IMGUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();               // Controlla compatibilità versione ImGui
    ImGui::CreateContext();             // Crea il contesto ImGui (necessario)
    ImGuiIO& io = ImGui::GetIO();       // Ottiene l’oggetto IO di ImGui (config e input)
    io.FontGlobalScale = 2.0f;          // Scala globale del font (doppio delle dimensioni)
    ImGui::StyleColorsDark();            // Imposta tema scuro predefinito
    
    // Inizializza l’integrazione con GLFW per OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core"); // Inizializza renderer OpenGL3 con versione 330 core
}

// Funzione di rendering dell’interfaccia grafica per ogni frame
void my_interface()
{
    ImGui_ImplGlfw_NewFrame();          // Prepara frame nuovo per input GLFW
    ImGui_ImplOpenGL3_NewFrame();       // Prepara frame nuovo per rendering OpenGL3
    ImGui::NewFrame();                  // Inizia registrazione nuova UI frame

    ImGui::SetNextWindowPos(ImVec2(50, height*0.9)); // Imposta posizione finestra successiva (50, 1050)

    ImGui::Begin("Impostazioni Sfondo", NULL,  // Apre finestra "Impostazioni"
        ImGuiWindowFlags_NoResize |      // Impedisce ridimensionamento utente
        ImGuiWindowFlags_AlwaysAutoResize | // Ridimensionamento automatico al contenuto
        ImGuiWindowFlags_NoBackground |  // Sfondo trasparente
        ImGuiWindowFlags_NoTitleBar |    // Nasconde barra titolo
        ImGuiWindowFlags_NoMove           // Impedisce lo spostamento finestra
    );
    /*
    I flag sono valori interi con un solo bit attivato; combinati col bitwise OR
    abilitano più opzioni contemporaneamente. ImGui controlla quali flag sono attivi
    con operazioni bit a bit per definire il comportamento finestra.
    */
   
    ImGui::ColorEdit4("Colore di Sfondo", (float*)&clear_color); // Widget per modificare colore sfondo
    ImGui::Text("Gestione dei punti");
    ImGui::RadioButton("Inserimento", &Mod, 0); ImGui::SameLine();
    ImGui::RadioButton("Modifica", &Mod, 1); ImGui::SameLine();
    ImGui::RadioButton("Elimina", &Mod, 2); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &flag_wireframe);
    { const ImVec2 p = ImGui::GetWindowPos(), z = ImGui::GetWindowSize();
      RegistraAreaPannello(p.x, p.y, z.x, z.y, 0); }
    ImGui::End(); // Chiude la finestra "Impostazioni"

    if (Mod == 0)  //Se sono in modalità inserimento, imposto che non c'è alcun punto selezionato
        selected_point = -1;

     
    ImGui::SetNextWindowPos(ImVec2(50, height*0.1), ImGuiCond_FirstUseEver);
    ImGui::Begin("Impostazioni Curva", NULL, ImGuiWindowFlags_AlwaysAutoResize); // Finestra con auto-resize  ); // Finestra con auto-resize  

    static int numeroRichiesto = 1;
    ImGui::InputInt("Numero di curve", &numeroRichiesto);
    if (numeroRichiesto < 1) numeroRichiesto = 1;
    if (numeroRichiesto > 20) numeroRichiesto = 20;
    if (ImGui::Button("Crea curve mancanti")) {
        CambiaNumeroCurve(numeroRichiesto);
        Mod = 0;
        modTg = false; // Disattiva la modifica tangenti quando si creano curve.
        selected_point = -1;
    }
    ImGui::Text("Curve disponibili: %d", static_cast<int>(curve.size()));
    if (curva) ImGui::Text("Curva attiva: %d | Punti: %d | Modalita: %s",
        curvaAttivaIndice + 1, static_cast<int>(curva->CP.size()),
        Mod == 0 ? "Inserimento" : (Mod == 1 ? "Modifica" : "Elimina"));
    for (int k = 0; k < static_cast<int>(curve.size()); ++k) {
        if (k > 0) ImGui::SameLine();
        char nome[32]; sprintf(nome, "Curva %d", k + 1);
        if (ImGui::RadioButton(nome, curvaAttivaIndice == k)) {
            SelezionaCurva(k);
            Mod = 0;
            modTg = false; // Non lasciare che la GUI forzi Mod=1.
            selected_point = -1;
        }
    }
    if (curva) {
        ImGui::ColorEdit3("Colore curva selezionata", &curva->coloreCurva.r);
    }
    ImGui::Separator();
    ImGui::RadioButton("Parametrizzazione Uniforme", &param, 0); ImGui::SameLine();
    ImGui::RadioButton("Parametrizzazione secondo le corde", &param, 1); 
    ImGui::Checkbox("Visualizza Poligono di Controllo", &visualizza_PC);
    ImGui::Checkbox("Renderizza", &riempi);  ImGui::SameLine();


    if (ImGui::Button("Chiudi Forma (C1)")) {
        ImpostaChiusuraC1(curva);
    }
    if (curva->chiusaC1)
        ImGui::Text("Chiusura C1 attiva");

    ImGui::Text("Gestione Tangenti");
    ImGui::Checkbox("Visualizza Tangenti", &visualizzaTg);  ImGui::SameLine();
    if (ImGui::Checkbox("Modifica Tangenti", &modTg) && modTg)
        Mod = 1; // Solo nel momento in cui la casella viene attivata.
   
    //Salvataggio dati   

    
    ImGui::Text("Salvataggio Dati");
    static char percorsoForma[256] = "myshape.txt";
    static std::string messaggioForma;
    ImGui::InputText("File forma", percorsoForma, sizeof(percorsoForma));
    if (ImGui::Button("Salva Forma")) {
        std::string errore;
        if (SalvaForma(percorsoForma, errore)) messaggioForma = "Forma salvata correttamente.";
        else messaggioForma = errore;
    }
    ImGui::SameLine();
    if (ImGui::Button("Apri Forma")) {
        std::string errore;
        if (ApriForma(percorsoForma, errore)) {
            selected_point = -1;
            numeroRichiesto = static_cast<int>(curve.size());
            messaggioForma = "Forma caricata correttamente.";
        } else messaggioForma = errore;
    }
    if (!messaggioForma.empty()) ImGui::TextWrapped("%s", messaggioForma.c_str());
    static char percorsoOBJ[256] = "myshape.obj";
    static std::string messaggioOBJ;
    ImGui::InputText("File OBJ", percorsoOBJ, sizeof(percorsoOBJ));
    if (ImGui::Button("Esporta Mesh OBJ/MTL")) {
        EsportaFormaOBJ(percorsoOBJ, messaggioOBJ);
    }
    if (!messaggioOBJ.empty()) ImGui::TextWrapped("%s", messaggioOBJ.c_str());
    ImGui::Separator();
    ImGui::Text("Singola curva (mycurve_N.txt)");
    if (ImGui::Button("Salva")) {
        std::ofstream outputFile(("mycurve_" + std::to_string(curvaAttivaIndice + 1) + ".txt").c_str());
         if (outputFile.is_open()) {
            outputFile << "# C1 " << (curva->chiusaC1 ? 1 : 0) << "\n";
            outputFile << "# RGB " << curva->coloreCurva.r << " " << curva->coloreCurva.g << " " << curva->coloreCurva.b << "\n";
            for (int i = 0; i < curva->CP.size(); i++)
                outputFile << curva->CP[i].x << "  " << curva->CP[i].y << "  " << curva->CP[i].z << "  " << curva->Tens[i] << "  " << curva->Bias[i] << "  " << curva->Cont[i] << std::endl;

            outputFile.close();
            std::cout << "Dati scritti con successo." << std::endl;
        }
    }
    ImGui::SameLine();

    //Apertura dile dai dati
    if (ImGui::Button("Apri")) {
        int i;
        struct Dati {
            float x;
            float y;
            float z;
            float Tens;
            float Bias;
            float Cont;
        };

        FILE* file = fopen(("mycurve_" + std::to_string(curvaAttivaIndice + 1) + ".txt").c_str(), "r");
        if (file == NULL) {
            perror("Impossibile aprire il file");
        }
        if (file == NULL) { ImGui::End(); ImGui::Render(); return; }
        // Vettore per memorizzare i dati
        struct Dati dati[1000]; // Supponiamo che ci siano al massimo 100 righe nel file
        int riga = 0;
        int closedFromFile = 0;
        { char header[128]; long offset = ftell(file);
          if (fgets(header, sizeof(header), file)) {
              if (sscanf(header, "# C1 %d", &closedFromFile) != 1) fseek(file, offset, SEEK_SET);
              else {
                  long rgbOffset = ftell(file);
                  if (fgets(header, sizeof(header), file)) {
                      float r, g, b;
                      if (sscanf(header, "# RGB %f %f %f", &r, &g, &b) == 3 &&
                          std::isfinite(r) && std::isfinite(g) && std::isfinite(b) &&
                          r >= 0.f && r <= 1.f && g >= 0.f && g <= 1.f && b >= 0.f && b <= 1.f)
                          curva->coloreCurva = vec4(r, g, b, 1.f);
                      else fseek(file, rgbOffset, SEEK_SET);
                  }
              }
          }
        }
        while (fscanf(file, "%f %f %f %f %f %f ", &dati[riga].x, &dati[riga].y, &dati[riga].z, &dati[riga].Tens, &dati[riga].Bias, &dati[riga].Cont) == 6) {
            // Incrementa l'indice della riga
            riga++;
            cout << riga << endl;
            // Puoi aggiungere un controllo qui per evitare il superamento dell'array dati
            if (riga >= 1000) {
                printf("Troppe righe nel file. L'array dati   stato completamente riempito.\n");
                break;
            }
        }
        // Chiudi il file
        fclose(file);
        
        curva->chiusaC1 = false;
        curva->CP.clear(); curva->colCP.clear(); curva->Tens.clear(); curva->Bias.clear(); curva->Cont.clear(); curva->Derivata.clear();
        for (int i = 0; i < riga; i++) {
            curva->CP.push_back(vec3(dati[i].x, dati[i].y, dati[i].z));
            curva->Tens.push_back(dati[i].Tens);
            curva->Bias.push_back(dati[i].Bias);
            curva->Cont.push_back(dati[i].Cont);
            curva->colCP.push_back(vec4(0.0, 0.0, 0.0, 1.0));
            curva->Derivata.push_back(vec3(0.0, 0.0, 0.0));
        
        }

        curva->ncp = curva->CP.size();
        curva->chiusaC1 = (closedFromFile != 0 && curva->CP.size() >= 4);

        UPDATE_VAO_DYNAMIC(curva,true);
    }


    { const ImVec2 p = ImGui::GetWindowPos(), z = ImGui::GetWindowSize();
      RegistraAreaPannello(p.x, p.y, z.x, z.y, 1); }
    ImGui::End(); // Termina la finestra

    // Render ImGui
    ImGui::Render(); // Renderizza gli elementi dell'interfaccia ImGui

}
    

// Funzione per chiudere e pulire ImGui correttamente
void close_GUI() {
    ImGui_ImplOpenGL3_Shutdown();   // Rilascia risorse renderer OpenGL3
    ImGui_ImplGlfw_Shutdown();      // Rilascia risorse integrazione GLFW
    ImGui::DestroyContext();        // Distrugge contesto ImGui liberando memoria
}