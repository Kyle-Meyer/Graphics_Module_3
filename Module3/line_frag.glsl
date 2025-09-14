#version 440 core

// Input from vertex shader (interpolated per-vertex color)
in vec4 vertex_color;

// Output - final fragment color
out vec4 FragColor;

void main()
{
    // Use the interpolated vertex color for smooth gradients
    FragColor = vertex_color;
}
