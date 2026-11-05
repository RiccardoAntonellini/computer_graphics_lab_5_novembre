#version 330 core


// Attributi extra solo per Curva (control points, facoltativi)
layout(location = 2) in vec3 cp;        // punto di controllo
layout(location = 3) in vec4 cpColor;   // colore del punto di controllo

uniform mat4 Projection, Model;

out vec4 ourColor;

void main()
{
    gl_Position = Projection * Model * vec4(cp, 1.0);
    ourColor=cpColor;
    
}
	
