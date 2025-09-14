#version 440 core

// Vertex attributes - 2D position and per-vertex color
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

// Uniform - orthographic projection matrix
uniform mat4 ortho_matrix;

// Output to fragment shader
out vec4 vertex_color;

void main()
{
    // Pass the per-vertex color to fragment shader for interpolation
    vertex_color = color;
    
    // Transform the 2D position to clip space using orthographic projection
    gl_Position = ortho_matrix * vec4(position, 0.0, 1.0);
}
