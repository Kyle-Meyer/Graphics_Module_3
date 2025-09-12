#version 410 core

layout (location = 0) out vec4 frag_color; // Output fragment color

void main() 
{
    if (dot(gl_PointCoord-0.5, gl_PointCoord-0.5) > 0.25)
        discard;
    else
        frag_color = vec4(gl_PointCoord.st, 0, 1);
}
