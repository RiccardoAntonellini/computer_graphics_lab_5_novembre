// ============================================================================
// shader/fragmentshaderC.glsl - GUIDA DIDATTICA
// Fragment shader: determina il colore finale del frammento.
// ============================================================================

#version 330 core

in vec4 vColor;
out vec4 FragColor;

void main()
{
    FragColor = vColor;
}
