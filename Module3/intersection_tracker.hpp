#ifndef __SCENE_INTERSECTION_TRACKER_HPP__
#define __SCENE_INTERSECTION_TRACKER_HPP__

#include "ngon_geometry_node.hpp"
#include "geometry/segment2.hpp"
#include "scene/color4.hpp"
#include <vector>
#include <memory>

// Forward declarations
namespace cg {
    class PointShaderNode;
    class PointNode;
}

namespace cg
{

class IntersectionTracker
{
public:
    /**
     * Constructor
     */
    IntersectionTracker();
    
    /**
     * Initialize with Module 2 point shader and register n-gons
     * @param point_shader Module 2 point shader node
     * @param ngons Vector of n-gon geometry nodes to test against
     * @return True if initialization successful
     */
    bool initialize(std::shared_ptr<PointShaderNode> point_shader,
                   const std::vector<std::shared_ptr<NGonGeometryNode>>& ngons);
    
    /**
     * Update intersections for the current draggable line
     * @param line_start Start point of the line
     * @param line_end End point of the line
     */
    void update_intersections(const Point2& line_start, const Point2& line_end);
    
    /**
     * Clear all intersection points
     */
    void clear_intersections();
    
    /**
     * Get the number of current intersection points
     */
    size_t get_intersection_count() const;

private:
    struct NGonInfo {
        std::shared_ptr<NGonGeometryNode> ngon;
        std::vector<LineSegment2> edges;
        Color4 intersection_color;
        float point_size;
    };
    
    // Use Module 2 point shader and node
    std::shared_ptr<PointShaderNode> point_shader_;
    std::shared_ptr<PointNode> intersection_points_;
    std::vector<NGonInfo> ngon_info_;
    std::vector<Point2> current_intersections_;  // Track current intersection points
    
    void calculate_and_update_intersections(const LineSegment2& line);
};

} // namespace cg

#endif // __SCENE_INTERSECTION_TRACKER_HPP__
