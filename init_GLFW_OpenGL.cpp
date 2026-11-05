#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
int init_gl_GLFW();

int  init_gl_GLFW()
{
/* Initialize the library */

if (!glfwInit())   //se glfwInit() dà valore falso, return -1
return -1;

//Imposta le proprietà del contesto e del profile
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//Abilita il double buffering

glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

}