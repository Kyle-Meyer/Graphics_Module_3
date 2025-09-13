#include "ngon_geometry_node.hpp"
#include "geometry/geometry.hpp"
#include "scene/scene.hpp"
#include <GL/glext.h>
#include <cmath>
#include <iostream>

namespace cg 
{

NGonGeometryNode::NGonGeometryNode(const Point2& center, int num_sides, float radius)
    : center_(center), num_sides_(num_sides), radius_(radius),
      vao_(0), vertex_buffer_(0), index_buffer_(0),
      vertex_count_(0), index_count_(0)
{
    // Ensure minimum of 3 sides
    if (num_sides_ < 3) {
        num_sides_ = 3;
        std::cout << "Warning: NGon requires at least 3 sides. Setting to 3." << std::endl;
    }
    
    node_type_ = SceneNodeType::GEOMETRY;
}

NGonGeometryNode::~NGonGeometryNode()
{
  destroy();
}

bool NGonGeometryNode::create()
{
  //generate vertex and index data 
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  generate_vertices(vertices, indices);

  vertex_count_ = static_cast<int>(vertices.size()) / 2; //2 components per vertex
  index_count_ = static_cast<int>(indices.size());
  
  //generate opengl objects
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &index_buffer_);

  glBindVertexArray(vao_);

  //upload vertex data
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, 
               vertices.size() * sizeof(float), 
               vertices.data(), 
               GL_STATIC_DRAW);

  //upload index data 
  glBindBuffer(GL_ARRAY_BUFFER, index_buffer_);
  glBufferData(GL_ARRAY_BUFFER,
               indices.size() * sizeof(unsigned int),
               indices.data(),
               GL_STATIC_DRAW);

  //setup vertex attributes (assuming position is at location 0)
  glVertexAttribPointer(0,2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  //unbind vao 
  glBindVertexArray(0);

  cg::check_error("NGonGeometryNode::create");
  return true;
}

void NGonGeometryNode::draw(SceneState& scene_state)
{
  if(vao_ == 0)
  {
    std::cout << "warning:: ngon not init, call create first" << std::endl;
    return;
  }

  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  cg::check_error("NGonGeometryNode::draw");

  SceneNode::draw(scene_state);
}

void NGonGeometryNode::destroy()
{
  if(vao_ != 0)
  {
    glDeleteVertexArrays(1, &vao_);
    vao_ = 0;
  }
  
  if(vertex_buffer_ != 0)
  {
    glDeleteBuffers(1, &vertex_buffer_);
    vertex_buffer_ = 0;
  }

  if(index_buffer_ != 0)
  {
    glDeleteBuffers(1, &index_buffer_);
    index_buffer_ = 0;
  }

  vertex_count_ = 0;
  index_count_ = 0;
}

void NGonGeometryNode::generate_vertices(std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
    vertices.clear();
    indices.clear();
    
    // Reserve space for efficiency
    vertices.reserve((num_sides_ + 1) * 2); // +1 for center vertex, *2 for x,y coordinates
    indices.reserve(num_sides_ * 3);        // 3 indices per triangle
    
    // Add center vertex first
    vertices.push_back(center_.x);
    vertices.push_back(center_.y);
    
    // Generate vertices around the circle
    for (int i = 0; i < num_sides_; ++i) {
        // Calculate angle for this vertex (counter-clockwise from positive X-axis)
        float angle = (2.0f * PI * i) / static_cast<float>(num_sides_);
        
        // Calculate vertex position
        float x = center_.x + radius_ * std::cos(angle);
        float y = center_.y + radius_ * std::sin(angle);
        
        vertices.push_back(x);
        vertices.push_back(y);
    }
    
    // Generate triangle indices using triangle fan approach
    // Each triangle connects center (index 0) with two consecutive perimeter vertices
    for (int i = 0; i < num_sides_; ++i) {
        indices.push_back(0);                           // Center vertex
        indices.push_back(i + 1);                       // Current perimeter vertex
        indices.push_back((i + 1) % num_sides_ + 1);    // Next perimeter vertex (wrap around)
    }
}

}
