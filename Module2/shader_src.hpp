#ifndef __MODULE2_SHADER_SRC_HPP__
#define __MODULE2_SHADER_SRC_HPP__

const char *lines_vert =
    R"(
#version 410 core
layout (location = 0) in vec2 vtx_position;
uniform mat4 ortho;
void main()
{
    gl_Position  = ortho * vec4(vtx_position, 0.0, 1.0);
}
)";

const char *lines_frag =
    R"(
#version 410 core
layout (location = 0) out vec4 frag_color;
uniform vec4 color; 
void main() 
{
    frag_color = color;
}
)";

const char *points_vert =
    R"(
#version 410 core
layout (location = 0) in vec2 vtx_position;
uniform mat4 ortho;
void main()
{
    gl_Position  = ortho * vec4(vtx_position, 0.0, 1.0);
}
)";

const char *points_frag =
    R"(
#version 410 core
layout (location = 0) out vec4 frag_color; // Output fragment color
void main() 
{
    if (dot(gl_PointCoord-0.5, gl_PointCoord-0.5) > 0.25) discard;
    else frag_color = vec4(gl_PointCoord.st, 0, 1);
}
)";

#endif
