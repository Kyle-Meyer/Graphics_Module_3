//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    line_node.hpp
//	Purpose: Simple geometry node that draws a connected line.
//
//============================================================================

#ifndef __MODULE2_LINE_NODE_HPP__
#define __MODULE2_LINE_NODE_HPP__

#include "scene/geometry_node.hpp"

#include "geometry/point2.hpp"
#include "scene/color4.hpp"

#include <vector>

namespace cg
{

/**
 * Unit sphere geometry node.
 */
class LineNode : public GeometryNode
{
  public:
    /**
     * Constructor.
     * @param  c     Color for the line.
     */
    LineNode(const Color4 &c);

    /**
     * Destructor. Delete VBO and VAO.
     */
    ~LineNode();

    /**
     * Adds a point to the list. Reloads the point VBO.
     * @param  x   Screen x location
     * @param  y   Screen y location
     * @param  position_loc  Position location (shader vertex attribute)
     */
    void add(float x, float y, int32_t position_loc);

    /**
     * Draw the lines
     * @param  scene_state  Current scene state.
     */
    void draw(SceneState &scene_state) override;

  protected:
    Color4              color_;       // Color of the line
    GLuint              vbo_;         // VBO
    GLuint              vao_;         // Vertex Array Object
    std::vector<Point2> vertex_list_; // Vertex list
};

} // namespace cg

#endif
