#ifndef __SCENE_NGON_GEOMETRY_NODE_HPP__
#define __SCENE_NGON_GEOMETRY_NODE_HPP__

#include "scene/geometry_node.hpp"
#include "geometry/point2.hpp"
#include <vector>
#include "geometry/segment2.hpp"

namespace cg 
{

class NGonGeometryNode : public GeometryNode
{
public:
  //constructor
  NGonGeometryNode(const Point2& center, int num_sides, float radius);

  //destructor
  virtual ~NGonGeometryNode();

  //create method 
  /**
   * Create the OpenGL buffers and generate geometry
   * @return Returns true if successful
   */
  virtual bool create();

  /**
   * Draw the n-gon
   * @param scene_state Current scene state
   */
  virtual void draw(SceneState& scene_state) override;

  /**
   * Clean up OpenGL resources
   */
  virtual void destroy();

  /**
   * Get the perimeter edges of the n-gon as LineSegment2 objects
   * @param edges Vector to store the edge line segments
   */
  void get_perimeter_edges(std::vector<cg::LineSegment2>& edges) const;
  
  /**
   * Get the center point of the n-gon
   * @return Center point in world coordinates
   */
  const Point2& get_center() const { return center_; }
  
  /**
   * Get the radius of the n-gon
   * @return Radius value
   */
  float get_radius() const { return radius_; }
  
  /**
   * Get the number of sides
   * @return Number of sides
   */
  int get_num_sides() const { return num_sides_; }

private:
  Point2 center_;
  int num_sides_;
  float radius_;
  
  // OpenGL buffer objects
  GLuint vao_;           // Vertex Array Object
  GLuint vertex_buffer_; // Vertex Buffer Object
  GLuint index_buffer_;  // Element Buffer Object (for indices)
  
  int vertex_count_;
  int index_count_;
  
  /**
   * Generate vertex data for the n-gon using triangle fan approach
   */
  void generate_vertices(std::vector<float>& vertices, std::vector<unsigned int>& indices);
};

}

#endif // !__SCENE_NGON_GEOMETRY_NODE_HPP__
