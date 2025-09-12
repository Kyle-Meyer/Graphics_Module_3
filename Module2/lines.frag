#version 410 core

layout (location = 0) out vec4 frag_color; // Output fragment color

uniform vec4 color;    // Constant input color

void main() 
{
    frag_color = color;
}
