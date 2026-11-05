#version 330 core

// Attributi comuni Figura/Curva
layout(location = 4) in vec3 aPos;      // posizione dei vertici delle tangenti
layout(location = 5) in vec4 aColor;    // colore dei vertici delle tangenti
 

 
uniform mat4 Projection, Model;
 
out vec4 ourColor;

 

void main()
{
     
     

    gl_Position = Projection * Model * vec4(aPos, 1.0);

    ourColor=aColor; 
}
	
