#include "basic_shader_node.hpp"
#include "scene/scene.hpp"
#include "scene/scene_state.hpp"
#include <GL/glext.h>
#include <iostream>

namespace cg
{

BasicShaderNode::BasicShaderNode()
{
    // Constructor - nothing special needed beyond base class
}

BasicShaderNode::~BasicShaderNode()
{
    // Destructor - base class handles cleanup
}

bool BasicShaderNode::create()
{
    if(!ShaderNode::create("Module3/basic_vert.glsl", "Module3/basic_frag.glsl"))
    {
        std::cout << "Basic Node, failed to create shader program from files" << std::endl;
        return false; 
    } 

    // Check if shader program is valid
    GLuint program = shader_program_.get_program();
    std::cout << "Shader program ID: " << program << std::endl;
    
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    std::cout << "Shader link status: " << (link_status == GL_TRUE ? "SUCCESS" : "FAILED") << std::endl;
    
    if (link_status != GL_TRUE) {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        if (log_length > 0) {
            std::vector<char> log(log_length);
            glGetProgramInfoLog(program, log_length, nullptr, log.data());
            std::cout << "Shader link error: " << log.data() << std::endl;
        }
    }

    if(!get_locations())
    {
        std::cout << "Basic shader node failed to get shader locations" << std::endl;
        return false;
    }

    std::cout << "BasicShaderNode created successfully!" << std::endl;
    return true;
}

bool BasicShaderNode::get_locations()
{
  GLuint program = shader_program_.get_program();

  if(program == 0)
  {
    std::cout << "basic shader node get_locations failed to get shader program!" << std::endl;
    return false;
  }

  GLint pos_loc = glGetAttribLocation(program, "position");
  if(pos_loc == -1)
  {
    std::cout << "BasicShaderNode: could not find position attribute" << std::endl;
    return false;
  }

  GLint ortho_loc = glGetUniformLocation(program, "ortho_matrix");
  if(ortho_loc == -1)
  {
    std::cout << "BasicShaderNode: could not get ortho matrix uniform" << std::endl;
    return false; 
  }

  GLint color_loc = glGetUniformLocation(program, "color");
  if(color_loc == -1)
  {
    std::cout << "BasicShaderNode: could not get color uniform" << std::endl;
    return false;
  }

  std::cout << "BasicShaderNode: Found all shader locations:" << std::endl;
  std::cout << "  position attribute: " << pos_loc << std::endl;
  std::cout << "  ortho_matrix uniform: " << ortho_loc << std::endl;
  std::cout << "  color uniform: " << color_loc << std::endl;

  return true;
}

void BasicShaderNode::draw(SceneState& scene_state)
{

  shader_program_.use();
  cg::check_error("BasicShaderNode::draw - use shader");
  cg::check_error("BasicShaderNode::draw");
  // Update scene state with our locations (in case they're not set)
  GLuint program = shader_program_.get_program();
  scene_state.position_loc = glGetAttribLocation(program, "position");
  scene_state.ortho_matrix_loc = glGetUniformLocation(program, "ortho_matrix");
  scene_state.color_loc = glGetUniformLocation(program, "color");

  // Set the orthographic matrix uniform
  if (scene_state.ortho_matrix_loc != -1)
  {
      glUniformMatrix4fv(scene_state.ortho_matrix_loc, 1, GL_FALSE, scene_state.ortho.data());
      cg::check_error("BasicShaderNode::draw - set ortho matrix");
  }

  // Draw all children with this shader active
  SceneNode::draw(scene_state);
  
  cg::check_error("BasicShaderNode::draw - end");
}

}
