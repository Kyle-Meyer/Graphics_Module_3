#version 440 core

// Uniform - color for all fragments
uniform vec4 color;

// Output - final fragment color
out vec4 FragColor;

void main()
{
    // Apply the uniform color to this fragment
    FragColor = color;
}
