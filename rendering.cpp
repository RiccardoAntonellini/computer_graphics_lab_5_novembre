#pragma once
#include <glad/glad.h>
#include "ShaderMaker.h"
#include "strutture.h"
#include "init_geometrie.h"
#include "inizializzazioni.h"
#include "gestione_curve.h"
#include "earcut.hpp"
#include "gestione_multicurva.h"  //Serve per la triangolazione della curva
#include "Gui.h"
#include <array>
#include "imgui.h"           // Libreria core ImGui
#include "imgui_impl_glfw.h" // Integrazione ImGui con GLFW (gestione finestra/input)
#include "imgui_impl_opengl3.h" // Integrazione ImGui con OpenGL3 (rendering)
#include "lib.h"
#define PI 3.14159265358979323
extern int  width, height;
 
extern ImVec4 clear_color;
bool showMenu = false, visualizza_PC = true;;
double mousex, mousey; //Coordinate del mouse
bool flag_wireframe;

// Identificatori delle variabili uniformi:  Model  e Projection
extern GLuint IdProj, IdModel, IdCurve, IdColorA,IdColorB, IdProj_TG, IdModel_TG, IdProj_CP, IdModel_CP;
extern GLuint IdProj_Triangul, IdModel_Triangul;
extern unsigned int programId, programId_CP, programId_Triangul, programId_TG;
extern Figura assi;
extern Curva* curva;
bool visualizzaTg, modTg, riempi, tangenteModificata;
//Inizializzazione parametrizzazione e Modalità dle mouse.
int param=0, Mod=0;
extern mat4 Projection; // matrice di proiezione: dalle coordinate del mondo alle coordinate di display normalizzate
float* t;
using Coord = float;
using Point2D = std::array<Coord, 2>;

void triangolarizzazzione_e_rendering(Curva* curva)
{
    vector<Point2D> polygon2D;
    polygon2D.reserve(curva->vertices.size());
    
    for (const auto& v : curva->vertices)  
        polygon2D.push_back(Point2D{ v.x, v.y });
     

    vector<vector<Point2D>> polygon = { polygon2D };
    // Triangolazione facebndo uso della funzione eartcut
    vector<uint32_t> indices = mapbox::earcut<uint32_t>(polygon);
    // Output degli indici dei triangoli
    curva->indices = indices;
    UPDATE_VAO_DYNAMIC(curva);
    // Colore specifico della curva, aggiornato a ogni frame da ImGui.
    const vec4 colorA = curva->coloreCurva;
    // Tonalita piu chiara per la sfumatura della triangolazione.
    const float schiarimento = 0.35f;
    const vec4 colorB = vec4(
        colorA.r + (1.0f - colorA.r) * schiarimento,
        colorA.g + (1.0f - colorA.g) * schiarimento,
        colorA.b + (1.0f - colorA.b) * schiarimento,
        colorA.a);
    glUseProgram(programId_Triangul);
    glUniform4f(IdColorA, colorA.r, colorA.g, colorA.b, colorA.a);
    glUniform4f(IdColorB, colorB.r, colorB.g, colorB.b, colorB.a);
    glUniformMatrix4fv(IdProj_Triangul, 1, GL_FALSE, value_ptr(Projection));
    glUniformMatrix4fv(IdModel_Triangul, 1, GL_FALSE, value_ptr(curva->Model));
    glBindVertexArray(curva->VAO);
    glDrawElements(GL_TRIANGLES, curva->indices.size(), GL_UNSIGNED_INT, 0);  //Modalità di rendering GL_LINE_STRIP: tutti i vertici vengono uniti da una spezzata
    glBindVertexArray(0);
}
 
void renderizza(float currentTime){

 

    int i;
    float step_t;
    glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT); // Pulisce il buffer colore

    
 
        
     if (flag_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

     glLineWidth(4.0);
     glPointSize(10.0);
 
     //Disegno gli assi
     glUseProgram(programId);
     glUniformMatrix4fv(IdProj, 1, GL_FALSE, value_ptr(Projection));
     glUniformMatrix4fv(IdModel, 1, GL_FALSE, value_ptr(assi.Model));
     glBindVertexArray(assi.VAO);
     //Chiama glDrawArrays con la modalità specifica(Scena[i].render), a partire dal primo vertice, 
      glDrawArrays(assi.render, 0, assi.nv);
      
      for (const auto& elemento : curve) {
      Curva* curva = elemento.get();
      //Visualizza vertici di controllo e poligono di controllo
      if (curva == ::curva) {
        glUseProgram(programId_CP);
        //Attiva il VBO_vertices dela curva
        UPDATE_VAO_DYNAMIC(curva, true);
        glUniformMatrix4fv(IdProj_CP, 1, GL_FALSE, value_ptr(Projection));
        glUniformMatrix4fv(IdModel_CP, 1, GL_FALSE, value_ptr(assi.Model));
        glBindVertexArray(curva->VAO);
        glDrawArrays(GL_LINE_STRIP, 0, curva->CP.size());  //Disegna i vertici di controllo della curva come punti isolati
        glDrawArrays(GL_POINTS, 0, curva->CP.size());
        //Visualizzazione dei vertici come punti
        glBindVertexArray(0);
     }
        //Se il numero dei vertici di controllo (CP) è maggiore di 2, calcola la curva di Hermite che li interpola.
        
        
      if (curva->CP.size() > 2)
      {

          //Scelta della parametrizzazione
          if (param == 0)
              t_equidistanti(curva);
          else
              t_corde(curva);

          glUseProgram(programId);
          curva->Model = mat4(1.0);
          //Calcola la curva di Hermite
          CostruisciHermite(curva);
          //Aggiorna il VAO della curva
          UPDATE_VAO_DYNAMIC(curva, true);
          //Visualizza la curva
          //Non aggiorno le variabili uniform per programID perchè non variano e sono state già passate quando è stato usato per il siegno degli assi.
          glBindVertexArray(curva->VAO);
          glDrawArrays(GL_LINE_STRIP, 0, curva->vertices.size());  //Modalità di rendering GL_LINE_STRIP: tutti i vertici vengono uniti da una spezzata
          glBindVertexArray(0);

          //Visualizza le tantgenti  nei vertici di controlllo.
          if (visualizzaTg == 1 && curva == ::curva)
          {
              UPDATE_VAO_DYNAMIC(curva, true);
              glUseProgram(programId_TG);
              glUniformMatrix4fv(IdProj_TG, 1, GL_FALSE, value_ptr(Projection));
              glUniformMatrix4fv(IdModel_TG, 1, GL_FALSE, value_ptr(assi.Model));
              glBindVertexArray(curva->VAO);
              glDrawArrays(GL_LINES, 0, curva->tg.size());  //Modalità di rendering GL_LINES: i vertici nel VAO a due a due vengono collegati con un segmento
              glBindVertexArray(0);
          }

          //Triangolazione della curva mediante libreria earcut e rendering della curva triangolarizzata
          if (riempi)
              triangolarizzazzione_e_rendering(curva);

      }

      } // Tutte le curve sono disegnate, solo quella selezionata riceve input.

      //Attivazione/Disattivazione IMGUI con tasto F1
      if (showMenu) {
          my_interface(); // Disegna l’interfaccia ImGui
          /* Renderizza la GUI ImGui */
          ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      }
}

 