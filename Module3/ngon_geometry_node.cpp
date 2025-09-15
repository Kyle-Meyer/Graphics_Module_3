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
        std::cout << "Warning: NGon requires at least 3 sides. Setting to 3." << "\n";
    }
    
    node_type_ = SceneNodeType::GEOMETRY;
}

NGonGeometryNode::~NGonGeometryNode()
{
  destroy();
}

bool NGonGeometryNode::create()
{
  // Generate vertex and index data 
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  generate_vertices(vertices, indices);

  vertex_count_ = static_cast<int>(vertices.size()) / 2; // 2 components per vertex
  index_count_ = static_cast<int>(indices.size());
  
  std::cout << "NGon has " << vertex_count_ << " vertices and " << index_count_ << " indices" << "\n";
  
  // Print first few vertices for debugging
  std::cout << "First few vertices: ";
  for (size_t i = 0; i < std::min((size_t)10, vertices.size()); i++) {
      std::cout << vertices[i] << " ";
  }
  std::cout << "\n";
  
  // Generate OpenGL objects
  std::cout << "About to generate VAO..." << "\n";
  glGenVertexArrays(1, &vao_);
  cg::check_error("glGenVertexArrays");

  std::cout << "Generated VAO: " << vao_ << "\n";
  if (vao_ == 0) {
      std::cout << "ERROR: VAO generation failed!" << "\n";
      return false;
  }

  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &index_buffer_);

  // Bind VAO first
  glBindVertexArray(vao_);
  cg::check_error("glBindVertexArray");

  // Upload vertex data
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  cg::check_error("glBindBuffer vertex");
  
  glBufferData(GL_ARRAY_BUFFER, 
               vertices.size() * sizeof(float), 
               vertices.data(), 
               GL_STATIC_DRAW);
  cg::check_error("glBufferData vertex");

  // Upload index data 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
  cg::check_error("glBindBuffer index");
  
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               indices.size() * sizeof(unsigned int),
               indices.data(),
               GL_STATIC_DRAW);
  cg::check_error("glBufferData index");

  // Setup vertex attributes (position at location 0)
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  cg::check_error("glVertexAttribPointer");
  
  glEnableVertexAttribArray(0);
  cg::check_error("glEnableVertexAttribArray");

  // Unbind VAO (good practice)
  glBindVertexArray(0);
  
  // Verify the setup worked
  GLint max_vertex_attribs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);
  std::cout << "Max vertex attributes: " << max_vertex_attribs << "\n";

  cg::check_error("NGonGeometryNode::create - end");
  
  std::cout << "NGonGeometryNode::create completed successfully" << "\n";
  return true;
}

void NGonGeometryNode::draw(SceneState& scene_state)
{
    if(vao_ == 0)
    {
        std::cout << "Warning: NGon not initialized, call create() first" << "\n";
        return;
    }

    
    // Verify the current shader program
    GLint current_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
    
    // Check if vertex attribute 0 is enabled
    GLint vertex_attrib_enabled;
    glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &vertex_attrib_enabled);
    
    // Bind VAO and draw
    glBindVertexArray(vao_);
    cg::check_error("NGonGeometryNode::draw - bind VAO");
    
    // Verify VAO is bound
    GLint bound_vao;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &bound_vao);
    
    // Check if depth testing might be interfering
    GLboolean depth_test_enabled = glIsEnabled(GL_DEPTH_TEST);
    
    // Draw the elements
    glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, 0);
    cg::check_error("NGonGeometryNode::draw - glDrawElements");
    
    // Re-enable depth test if it was enabled
    if (depth_test_enabled) {
        glEnable(GL_DEPTH_TEST);
    }
    
    glBindVertexArray(0);
    cg::check_error("NGonGeometryNode::draw - unbind VAO");

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
    
    std::cout << "Generating " << num_sides_ << "-gon at (" << center_.x << ", " << center_.y 
              << ") with radius " << radius_ << "\n";
    
    // Reserve space for efficiency
    vertices.reserve((num_sides_ + 1) * 2); // +1 for center vertex, *2 for x,y coordinates
    indices.reserve(num_sides_ * 3);        // 3 indices per triangle
    
    // Add center vertex first
    vertices.push_back(center_.x);
    vertices.push_back(center_.y);
    std::cout << "Center vertex: (" << center_.x << ", " << center_.y << ")" << "\n";
    
    // Generate vertices around the circle
    std::cout << "Perimeter vertices:" << "\n";
    for (int i = 0; i < num_sides_; ++i) {
        // Calculate angle for this vertex (counter-clockwise from positive X-axis)
        float angle = (2.0f * PI * i) / static_cast<float>(num_sides_);
        
        // Calculate vertex position
        float x = center_.x + radius_ * std::cos(angle);
        float y = center_.y + radius_ * std::sin(angle);
        
        vertices.push_back(x);
        vertices.push_back(y);
        
        if (i < 8) { // Only print first 8 to avoid spam
            std::cout << "  Vertex " << i << ": (" << x << ", " << y << ") at angle " << angle << " radians" << "\n";
        }
    }
    
    // Generate triangle indices using triangle fan approach
    std::cout << "Triangle indices:" << "\n";
    for (int i = 0; i < num_sides_; ++i) 
  {
        unsigned int center_idx = 0;
        unsigned int current_idx = i + 1;
        unsigned int next_idx = (i + 1) % num_sides_ + 1;
        
        indices.push_back(center_idx);      // Center vertex
        indices.push_back(current_idx);     // Current perimeter vertex
        indices.push_back(next_idx);        // Next perimeter vertex (wrap around)
        
        if (i < 4) { // Only print first 4 triangles to avoid spam
            std::cout << "  Triangle " << i << ": [" << center_idx << ", " << current_idx << ", " << next_idx << "]" << "\n";
        }
    }
    
    std::cout << "Generated " << vertices.size()/2 << " vertices and " << indices.size()/3 << " triangles" << "\n";
}


void NGonGeometryNode::get_perimeter_edges(std::vector<cg::LineSegment2>& edges) const
{
    edges.clear();
    edges.reserve(num_sides_);
    
    std::cout << "Extracting " << num_sides_ << " edges from n-gon at (" 
              << center_.x << ", " << center_.y << ") with radius " << radius_ << "\n";
    
    // Generate the same perimeter vertices as in generate_vertices()
    std::vector<Point2> perimeter_points;
    perimeter_points.reserve(num_sides_);
    
    for (int i = 0; i < num_sides_; ++i) {
        // Calculate angle for this vertex (counter-clockwise from positive X-axis)
        float angle = (2.0f * PI * i) / static_cast<float>(num_sides_);
        
        // Calculate vertex position
        float x = center_.x + radius_ * std::cos(angle);
        float y = center_.y + radius_ * std::sin(angle);
        
        perimeter_points.emplace_back(x, y);
    }
    
    // Create line segments between consecutive perimeter points
    for (int i = 0; i < num_sides_; ++i) {
        const Point2& current = perimeter_points[i];
        const Point2& next = perimeter_points[(i + 1) % num_sides_]; // Wrap around
        
        edges.emplace_back(current, next);
        
        if (i < 4) { // Only print first 4 edges to avoid spam
            std::cout << "  Edge " << i << ": (" << current.x << ", " << current.y 
                      << ") -> (" << next.x << ", " << next.y << ")" << "\n";
        }
    }
    
    std::cout << "Extracted " << edges.size() << " perimeter edges" << "\n";
}

}
