#version 440 core

// Vertex attribute - 2D position
layout(location = 0) in vec2 position;

// Uniform - orthographic projection matrix
uniform mat4 ortho_matrix;

void main()
{
    // Transform the 2D position to clip space using orthographic projection
    gl_Position = ortho_matrix * vec4(position, 0.0, 1.0);
}
