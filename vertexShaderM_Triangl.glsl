#version 330 core


 
layout(location = 0) in vec3 aPos;      // Posizione del vertice nello spazio 3D
layout(location = 6) in float aT;       // Parametro scalare usato per interpolare il colore

uniform mat4 Projection, Model;
uniform vec4 colorA;  // Colore di partenza (es. centro)
uniform vec4 colorB;  // Colore di destinazione (es. bordo)

out vec4 ourColor;

void main()
{
    gl_Position = Projection * Model * vec4(aPos, 1.0);

    // Interpola linearmente il colore tra colorA e colorB usando il parametro aT
    ourColor=mix(colorA,colorB,aT);
    
}
	
