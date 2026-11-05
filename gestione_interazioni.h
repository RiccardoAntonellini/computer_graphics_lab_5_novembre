#pragma once
#include "lib.h"
 
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
 void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
 void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void GestisciInserimentoCurve(GLFWwindow* window);

// Registra le aree dei pannelli ImGui (coordinate GLFW, origine in alto a sinistra).
void RegistraAreaPannello(float x, float y, float larghezza, float altezza, int indice);

// Svuota le aree ImGui quando la GUI viene nascosta con F1.
void CancellaAreePannelli();

void GestisciTrascinamentoCurve(GLFWwindow* window);
