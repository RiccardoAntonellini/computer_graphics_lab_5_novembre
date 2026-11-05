// ============================================================================
// shader/vertexshaderC.glsl - GUIDA DIDATTICA
// Vertex shader: riceve attributi di posizione e colore e produce la posizione di clip.
// ============================================================================

#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

out vec4 vColor;

void main()
{
    gl_Position = vec4(aPosition, 1.0);
    vColor = aColor;
}
