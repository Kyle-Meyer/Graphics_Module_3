#include "line_shader_node.hpp"
#include "scene/scene.hpp"
#include "scene/scene_state.hpp"
#include <iostream>

namespace cg
{

LineShaderNode::LineShaderNode() : color_attr_loc_(-1)
{
    // Constructor - initialize color attribute location
}

LineShaderNode::~LineShaderNode()
{
    // Destructor - base class handles cleanup
}

bool LineShaderNode::create()
{
    // Use the line-specific shaders that support per-vertex colors
    if(!ShaderNode::create("Module3/line_vert.glsl", "Module3/line_frag.glsl"))
    {
        std::cout << "LineShaderNode: Failed to create shader program from files" << "\n";
        return false;
    }

    // Check if shader program is valid
    GLuint program = shader_program_.get_program();
    std::cout << "LineShaderNode: Shader program ID: " << program << "\n";
    
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    std::cout << "LineShaderNode: Shader link status: " << (link_status == GL_TRUE ? "SUCCESS" : "FAILED") << "\n";
    
    if (link_status != GL_TRUE) {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        if (log_length > 0) {
            std::vector<char> log(log_length);
            glGetProgramInfoLog(program, log_length, nullptr, log.data());
            std::cout << "LineShaderNode: Shader link error: " << log.data() << "\n";
        }
        return false;
    }

    if(!get_locations())
    {
        std::cout << "LineShaderNode: Failed to get shader locations" << "\n";
        return false;
    }

    std::cout << "LineShaderNode: Created successfully!" << "\n";
    return true;
}

bool LineShaderNode::get_locations()
{
    GLuint program = shader_program_.get_program();

    if(program == 0)
    {
        std::cout << "LineShaderNode: get_locations failed - no shader program!" << "\n";
        return false;
    }

    // Get position attribute location (location 0)
    GLint pos_loc = glGetAttribLocation(program, "position");
    if(pos_loc == -1)
    {
        std::cout << "LineShaderNode: Could not find 'position' attribute" << "\n";
        return false;
    }

    // Get color attribute location (location 1)
    color_attr_loc_ = glGetAttribLocation(program, "color");
    if(color_attr_loc_ == -1)
    {
        std::cout << "LineShaderNode: Could not find 'color' attribute" << "\n";
        return false;
    }

    // Get orthographic matrix uniform location
    GLint ortho_loc = glGetUniformLocation(program, "ortho_matrix");
    if(ortho_loc == -1)
    {
        std::cout << "LineShaderNode: Could not find 'ortho_matrix' uniform" << "\n";
        return false;
    }

    std::cout << "LineShaderNode: Found all shader locations:" << "\n";
    std::cout << "  position attribute: " << pos_loc << "\n";
    std::cout << "  color attribute: " << color_attr_loc_ << "\n";
    std::cout << "  ortho_matrix uniform: " << ortho_loc << "\n";

    return true;
}

void LineShaderNode::draw(SceneState& scene_state)
{
    // Activate the shader program
    shader_program_.use();
    cg::check_error("LineShaderNode::draw - use shader");

    // Update scene state with our locations
    GLuint program = shader_program_.get_program();
    scene_state.position_loc = glGetAttribLocation(program, "position");
    scene_state.ortho_matrix_loc = glGetUniformLocation(program, "ortho_matrix");
    scene_state.color_loc = glGetUniformLocation(program, "color");

    // Set the orthographic matrix uniform
    if (scene_state.ortho_matrix_loc != -1)
    {
        glUniformMatrix4fv(scene_state.ortho_matrix_loc, 1, GL_FALSE, scene_state.ortho.data());
        cg::check_error("LineShaderNode::draw - set ortho matrix");
    }

    // Set line width for smooth, thick lines
    glLineWidth(4.0f);
    cg::check_error("LineShaderNode::draw - set line width");

    // Enable line smoothing for better quality (if supported)
    if (glIsEnabled(GL_LINE_SMOOTH) == GL_FALSE) {
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        cg::check_error("LineShaderNode::draw - enable line smoothing");
    }

    // Draw all children with this shader active
    SceneNode::draw(scene_state);
    
    // Reset line width to default
    glLineWidth(1.0f);
    
    cg::check_error("LineShaderNode::draw - end");
}

} // namespace cg
