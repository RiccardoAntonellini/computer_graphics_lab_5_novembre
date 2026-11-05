#version 330 core
 
layout(location = 0) in vec3 aPos;      // posizione dei vertici
layout(location = 1) in vec4 aColor;    // colore dei vertici
 
uniform mat4 Projection, Model;
out vec4 ourColor;

void main()
{
    gl_Position = Projection * Model * vec4(aPos, 1.0);
    ourColor = aColor;
   
}
	
