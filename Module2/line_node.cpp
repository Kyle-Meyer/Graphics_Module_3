#include "Module2/line_node.hpp"

#include "scene/scene.hpp"

namespace cg
{

LineNode::LineNode(const Color4 &c)
{
    // Copy the color
    color_ = c;

    // Create a buffer object and a vertex array object
    glGenBuffers(1, &vbo_);
    glGenVertexArrays(1, &vao_);
}

LineNode::~LineNode()
{
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
}

void LineNode::add(float x, float y, int32_t position_loc)
{
    vertex_list_.emplace_back(Point2(x, y));

    // Add the points to the VBO. Reloads entire VBO.
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 vertex_list_.size() * sizeof(Point2),
                 (GLvoid *)&vertex_list_[0],
                 GL_DYNAMIC_DRAW);

    // Update the VAO
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glEnableVertexAttribArray(position_loc);
    glVertexAttribPointer(position_loc, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
}

void LineNode::draw(SceneState &scene_state)
{
    // Draw line strip if at least 2 points
    if(vertex_list_.size() > 1)
    {
        // Set the color
        glUniform4f(scene_state.color_loc, color_.r, color_.g, color_.b, color_.a);

        // Bind the VAO and draw the line
        glBindVertexArray(vao_);
        glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(vertex_list_.size()));
        glBindVertexArray(0);
        check_error("End of Lines:");
    }
}

} // namespace cg
