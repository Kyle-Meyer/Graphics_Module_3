#ifndef __SCENE_DRAGGABLE_LINE_GEOMETRY_NODE_HPP__
#define __SCENE_DRAGGABLE_LINE_GEOMETRY_NODE_HPP__

#include "scene/geometry_node.hpp"
#include "geometry/point2.hpp"

namespace cg 
{

class DraggableLineGeometryNode : public GeometryNode
{
public:
    /**
     * Constructor
     * @param start_point Starting point of the line in world coordinates
     */
    DraggableLineGeometryNode(const Point2& start_point);

    /**
     * Destructor
     */
    virtual ~DraggableLineGeometryNode();

    /**
     * Create the OpenGL buffers and initialize line geometry
     * @return Returns true if successful
     */
    virtual bool create();

    /**
     * Draw the draggable line
     * @param scene_state Current scene state
     */
    virtual void draw(SceneState& scene_state) override;

    /**
     * Clean up OpenGL resources
     */
    virtual void destroy();

    /**
     * Update the end point of the line efficiently using glBufferSubData
     * @param end_point New end point in world coordinates
     */
    void update_end_point(const Point2& end_point);

    /**
     * Get the current start point
     * @return Current start point
     */
    const Point2& get_start_point() const { return start_point_; }

    /**
     * Get the current end point
     * @return Current end point
     */
    const Point2& get_end_point() const { return end_point_; }

private:
    // Line endpoints
    Point2 start_point_;
    Point2 end_point_;
    
    // OpenGL buffer objects
    GLuint vao_;           // Vertex Array Object
    GLuint vertex_buffer_; // Single VBO for interleaved position and color data
    
    // Vertex data structure for interleaved format
    struct LineVertex {
        float x, y;        // Position (2 floats)
        float r, g, b, a;  // Color (4 floats)
    };
    
    // Constants for buffer layout
    static constexpr int VERTICES_PER_LINE = 2;
    static constexpr int FLOATS_PER_VERTEX = 6; // 2 position + 4 color
    static constexpr size_t VERTEX_SIZE = sizeof(LineVertex);
    static constexpr size_t POSITION_OFFSET = 0;
    static constexpr size_t COLOR_OFFSET = 2 * sizeof(float);
    static constexpr size_t VERTEX_STRIDE = VERTEX_SIZE;
    
    // Colors for start and end points
    static constexpr float START_COLOR[4] = {0.8f, 0.1f, 0.1f, 1.0f}; // Red
    static constexpr float END_COLOR[4] = {0.1f, 0.8f, 0.1f, 1.0f};   // Green
    
    /**
     * Initialize vertex data for the line
     */
    void setup_vertex_data();
    
    /**
     * Setup vertex attribute pointers for interleaved data
     */
    void setup_vertex_attributes();
};

} // namespace cg

#endif // __SCENE_DRAGGABLE_LINE_GEOMETRY_NODE_HPP__
