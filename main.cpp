#include "lib.h"
#include "ShaderMaker.h"
#include "strutture.h" 
#include "inizializzazioni.h"
#include "init_geometrie.h"
#include "earcut.hpp"
#include "gestione_interazioni.h"
#include "Init_Finestra_Eventi.h"
#include <array>
#include "init_GLFW_OpenGL.h"
#include "imgui.h"           // Libreria core ImGui
#include "imgui_impl_glfw.h" // Integrazione ImGui con GLFW (gestione finestra/input)
#include "imgui_impl_opengl3.h" // Integrazione ImGui con OpenGL3 (rendering)
#include "rendering.h"
#include "Gui.h"
#include "gestione_multicurva.h"
#define PI 3.14159265358979323
//Variabili globali -------------------------------------------------------------------
unsigned int programId, programId_CP, programId_Triangul, programId_TG;  //Identificativo dello shader program
unsigned int IdProj_Triangul, IdModel_Triangul;
int height, width; //altezza e larghezza della finestra
GLFWwindow* window;

Figura assi;


ImVec4 clear_color = ImVec4(0.45f, 0.0f, 0.0f, 1.00f);  //Colore del background
mat4 Projection;

//Identificativi delle variabili uniform
GLuint IdProj, IdModel,IdCurve,IdColorA,IdColorB, IdProj_TG, IdModel_TG, IdProj_CP, IdModel_CP;
int selected_point = -1;  //Se non c'è alcun punto selezionato select_point=-1
 

//----------------------------------------------------------------------------------------
 
 


int main(void)
{ 
    //Inizializza GLFW, imposta le proprietà del contesto e del profile
    int flag_gl_GLFW= init_gl_GLFW();

    /* Inizializza la finestra opengl ed associa contesto e la gestione degli eventi */
    int flag= inizializza_window_context_events();
    //Inizializza l'intefaccia ImGUI
    Initialize_IMGUI(window);
    
    //Inizializza gli shader 
    INIT_SHADER();
  

    //Gestione della trasparenza:
    /*
    In OpenGL, la funzione glBlend serve a definire come combinare i colori di un pixel sorgente (che stiamo disegnabdo) e di un pixel destinazione (già presente nel framebuffer)
     quando si disegnano oggetti trasparenti.
    In questo caso specifico, glBlend(GL_SRC_ALPHA, GL_ONES_MINUS_SRC_ALPHA); indica che :
     GL_SRC_ALPHA: Il valore alfa del pixel sorgente(quello che stiamo disegnando) verrà utilizzato come fattore di miscelazione per il colore sorgente.
     GL_ONES_MINUS_SRC_ALPHA : Il valore alfa del pixel destinazione(quello già presente nel buffer) verrà sottratto da 1 e poi utilizzato come fattore di miscelazione
     per il colore destinazione. (già presente nel framebuffer
*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
    // Setto le coordinate dle mondo e la matrice di proiezione
    
    //In questo caso le coordinate del mondo variano sono in [-1,-1+x[1,1]
    Projection = ortho(-1.0,1.0,-1.0,1.0);
       
    //Location delle variabili uniform per i tre diversi shader

    //Shader per la visualizzazione dei vertici della curva o della figura
    IdProj = glGetUniformLocation(programId, "Projection");
    IdModel = glGetUniformLocation(programId, "Model");
    
    //Shader per la visualizzazione della renderizzazione della curva triangolata.

    IdColorA= glGetUniformLocation(programId_Triangul, "colorA");
    IdColorB= glGetUniformLocation(programId_Triangul, "colorB");
    IdModel_Triangul = glGetUniformLocation(programId_Triangul, "Model");
    IdProj_Triangul = glGetUniformLocation(programId_Triangul, "Projection");
    
    //Shader per la visualizzazione della renderizzazione delle tangenti nei vertici di controllo.
    IdModel_CP = glGetUniformLocation(programId_CP, "Model");
    IdProj_CP = glGetUniformLocation(programId_CP, "Projection");

    //Shader per la visualizzazione della renderizzazione delle tangenti nei vertici di controllo.
    IdModel_TG = glGetUniformLocation(programId_TG, "Model");
    IdProj_TG = glGetUniformLocation(programId_TG, "Projection");
    /* Loop until the user closes the window */
    INIT_ASSI(&assi);
    INIT_VAO(&assi);
    //Ver poter fare l'uodate del VAO bisogna inizializzare lo spazio ad un numero massimo che si è sicuri di non superare
    InizializzaCurve(1);

    while (!glfwWindowShouldClose(window))
    {
        
        float currentTime = glfwGetTime(); //rappresenta i secondi trascorsi dall’avvio di GLFW e cresce continuamente.

        //Utilizzare renderizza1(currentTime) per la soluzione del punto 1 dell'esercitazione;
        //  renderizza2(currentTime) per la soluzione del punto 2 dell'esercitazione, etc
        renderizza(currentTime);

        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        GestisciInserimentoCurve(window);
        GestisciTrascinamentoCurve(window);
    }
    /* Pulizia finale */

    close_GUI();               // Libera risorse ImGui
    glfwDestroyWindow(window); // Distrugge la finestra
    glDeleteProgram(programId);

    glfwTerminate();
    return 0;
}


  