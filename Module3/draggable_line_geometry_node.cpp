#include "draggable_line_geometry_node.hpp"
#include "scene/scene.hpp"
#include <iostream>

namespace cg 
{

// Static constant definitions
constexpr float DraggableLineGeometryNode::START_COLOR[4];
constexpr float DraggableLineGeometryNode::END_COLOR[4];

DraggableLineGeometryNode::DraggableLineGeometryNode(const Point2& start_point)
    : start_point_(start_point), end_point_(start_point), // Initialize end point same as start
      vao_(0), vertex_buffer_(0)
{
    node_type_ = SceneNodeType::GEOMETRY;
    std::cout << "DraggableLineGeometryNode: Created with start point (" 
              << start_point_.x << ", " << start_point_.y << ")" << "\n";
}

DraggableLineGeometryNode::~DraggableLineGeometryNode()
{
    destroy();
}

bool DraggableLineGeometryNode::create()
{
    std::cout << "DraggableLineGeometryNode: Creating OpenGL resources..." << "\n";
    
    // Generate OpenGL objects
    glGenVertexArrays(1, &vao_);
    cg::check_error("glGenVertexArrays");
    
    if (vao_ == 0) {
        std::cout << "ERROR: VAO generation failed!" << "\n";
        return false;
    }
    
    glGenBuffers(1, &vertex_buffer_);
    cg::check_error("glGenBuffers");
    
    std::cout << "Generated VAO: " << vao_ << ", VBO: " << vertex_buffer_ << "\n";
    
    // Bind VAO first
    glBindVertexArray(vao_);
    cg::check_error("glBindVertexArray");
    
    // Setup vertex data and attributes
    setup_vertex_data();
    setup_vertex_attributes();
    
    // Unbind VAO
    glBindVertexArray(0);
    
    std::cout << "DraggableLineGeometryNode: Created successfully!" << "\n";
    return true;
}

void DraggableLineGeometryNode::setup_vertex_data()
{
    // Create interleaved vertex data: [start_vertex, end_vertex]
    LineVertex vertices[VERTICES_PER_LINE] = {
        // Start vertex (red)
        {start_point_.x, start_point_.y, START_COLOR[0], START_COLOR[1], START_COLOR[2], START_COLOR[3]},
        // End vertex (green) - initially same as start
        {end_point_.x, end_point_.y, END_COLOR[0], END_COLOR[1], END_COLOR[2], END_COLOR[3]}
    };
    
    std::cout << "Setting up vertex data:" << "\n";
    std::cout << "  Start: (" << vertices[0].x << ", " << vertices[0].y 
              << ") Color: (" << vertices[0].r << ", " << vertices[0].g << ", " << vertices[0].b << ", " << vertices[0].a << ")" << "\n";
    std::cout << "  End: (" << vertices[1].x << ", " << vertices[1].y 
              << ") Color: (" << vertices[1].r << ", " << vertices[1].g << ", " << vertices[1].b << ", " << vertices[1].a << ")" << "\n";
    
    // Bind VBO and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    cg::check_error("glBindBuffer");
    
    // Allocate buffer with initial data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    cg::check_error("glBufferData");
    
    std::cout << "Uploaded " << sizeof(vertices) << " bytes to VBO" << "\n";
}

void DraggableLineGeometryNode::setup_vertex_attributes()
{
    // Position attribute (location 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*)POSITION_OFFSET);
    glEnableVertexAttribArray(0);
    cg::check_error("Position attribute setup");
    
    // Color attribute (location 1)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*)COLOR_OFFSET);
    glEnableVertexAttribArray(1);
    cg::check_error("Color attribute setup");
    
    std::cout << "Vertex attributes configured:" << "\n";
    std::cout << "  Position (loc 0): 2 floats, stride " << VERTEX_STRIDE << ", offset " << POSITION_OFFSET << "\n";
    std::cout << "  Color (loc 1): 4 floats, stride " << VERTEX_STRIDE << ", offset " << COLOR_OFFSET << "\n";
}

void DraggableLineGeometryNode::update_end_point(const Point2& end_point)
{
    end_point_ = end_point;
    
    // Create updated end vertex data
    LineVertex end_vertex = {
        end_point_.x, end_point_.y,
        END_COLOR[0], END_COLOR[1], END_COLOR[2], END_COLOR[3]
    };
    
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    
    // Efficiently update only the end vertex using glBufferSubData
    // Offset = sizeof(LineVertex) to skip the first vertex
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(LineVertex), sizeof(LineVertex), &end_vertex);
    cg::check_error("glBufferSubData - update end point");
    
    // Unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
}

void DraggableLineGeometryNode::draw(SceneState& scene_state)
{
  if(vao_ == 0)
  {
    std::cout << "Warning: DraggableLineGeometryNode not initialized, call create() first" << "\n";
    return;
  }
    
  if(!visible_)
  {
    return;
  }
  // Bind VAO and draw the line
  glBindVertexArray(vao_);
  cg::check_error("DraggableLineGeometryNode::draw - bind VAO");
  
  // Draw the line using GL_LINES primitive
  glDrawArrays(GL_LINES, 0, VERTICES_PER_LINE);
  cg::check_error("DraggableLineGeometryNode::draw - glDrawArrays");
  
  // Unbind VAO
  glBindVertexArray(0);
  cg::check_error("DraggableLineGeometryNode::draw - unbind VAO");
  
  // Call base class to draw any children (though lines typically have none)
  SceneNode::draw(scene_state);
}

void DraggableLineGeometryNode::destroy()
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
    
    std::cout << "DraggableLineGeometryNode: OpenGL resources cleaned up" << "\n";
}

void DraggableLineGeometryNode::reset_line(const Point2& start_point, const Point2& end_point)
{
    start_point_ = start_point;
    end_point_ = end_point;
    
    // Update both vertices in the buffer
    LineVertex vertices[VERTICES_PER_LINE] = {
        // Start vertex (red)
        {start_point_.x, start_point_.y, START_COLOR[0], START_COLOR[1], START_COLOR[2], START_COLOR[3]},
        // End vertex (green)
        {end_point_.x, end_point_.y, END_COLOR[0], END_COLOR[1], END_COLOR[2], END_COLOR[3]}
    };
    
    // Bind VBO and update entire buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    cg::check_error("DraggableLineGeometryNode::reset_line");
}

} // namespace cg
