#include "intersection_tracker.hpp"
#include "../Module2/point_shader_node.hpp"
#include "../Module2/point_node.hpp"
#include <iostream>

namespace cg
{

IntersectionTracker::IntersectionTracker()
    : point_shader_(nullptr), intersection_points_(nullptr)
{
    std::cout << "IntersectionTracker: Created" << "\n";
}

bool IntersectionTracker::initialize(std::shared_ptr<PointShaderNode> point_shader,
                                   const std::vector<std::shared_ptr<NGonGeometryNode>>& ngons)
{
    if (!point_shader) {
        std::cout << "IntersectionTracker: Error - null point shader" << "\n";
        return false;
    }
    
    point_shader_ = point_shader;
    
    // Create Module 2 point node for intersection points
    intersection_points_ = std::make_shared<PointNode>();
    
    // Add to point shader as a scene node (PointNode inherits from SceneNode)
    point_shader_->add_child(intersection_points_);
    
    
    for (size_t i = 0; i < ngons.size(); ++i) {
        if (!ngons[i]) {
            std::cout << "IntersectionTracker: Warning - null n-gon at index " << i << "\n";
            continue;
        }
        
        NGonInfo info;
        info.ngon = ngons[i];
        info.point_size = 28.0f + (i * 4.0f); // Vary size: 28, 32, 36, etc.
        
        // Cache the edges
        ngons[i]->get_perimeter_edges(info.edges);
        
        ngon_info_.push_back(info);
        
        std::cout << "IntersectionTracker: Registered '" << ngons[i]->get_name() 
                  << "' with " << info.edges.size() << " edges, color (" 
                  << info.intersection_color.r << ", " << info.intersection_color.g 
                  << ", " << info.intersection_color.b << "), size " << info.point_size << "\n";
    }
    
    std::cout << "IntersectionTracker: Initialized successfully with " 
              << ngon_info_.size() << " n-gons" << "\n";
    return true;
}

void IntersectionTracker::update_intersections(const Point2& line_start, const Point2& line_end)
{
    LineSegment2 line(line_start, line_end);
    calculate_and_update_intersections(line);
}

size_t IntersectionTracker::get_intersection_count() const
{
    return current_intersections_.size();
}

void IntersectionTracker::clear_intersections()
{
    if (point_shader_ && intersection_points_) {
        // Remove the current PointNode from the shader
        point_shader_->destroy(); // This clears all children
        
        // Recreate a fresh PointNode
        intersection_points_ = std::make_shared<PointNode>();
        point_shader_->add_child(intersection_points_);
        
        current_intersections_.clear();
    }
}

void IntersectionTracker::calculate_and_update_intersections(const LineSegment2& line)
{
  if (!point_shader_)
      return;
  
  // Always clear first to remove old points
  clear_intersections();
  
  size_t total_intersections = 0;
    
  // Test against each n-gon
  for (size_t ngon_idx = 0; ngon_idx < ngon_info_.size(); ++ngon_idx) 
  {
    const auto& ngon_info = ngon_info_[ngon_idx];
    size_t ngon_intersections = 0;
     
    // Test against each edge of this n-gon
    for (size_t edge_idx = 0; edge_idx < ngon_info.edges.size(); ++edge_idx) 
    {
      const auto& edge = ngon_info.edges[edge_idx];
         
      Segment2IntersectionResult result = line.intersect(edge);
         
      if (result.intersects) 
      {
        // Add intersection point to the SAME PointNode
        intersection_points_->add(result.intersect_point.x, 
                                  result.intersect_point.y, 
                                  point_shader_->get_position_loc());
             
        // Track the point for our own counting
        current_intersections_.push_back(result.intersect_point);
             
        ngon_intersections++;
        total_intersections++;
             

      }
    }
     
  }
}

} // namespace cg
