#version 410 core

// Per veretx attributes
layout (location = 0) in vec2 vtx_position;

// Uniform
uniform mat4 ortho_matrix;

void main()
{
    gl_PointSize = 20.0; 
    gl_Position  = ortho_matrix * vec4(vtx_position, 0.0, 1.0);
}
