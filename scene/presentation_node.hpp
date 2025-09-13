//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    presentation_node.hpp
//	Purpose: Scene graph presentation node.
//
//============================================================================

#ifndef __SCENE_PRESENTATION_NODE_HPP__
#define __SCENE_PRESENTATION_NODE_HPP__

#include "scene/scene_node.hpp"
#include "scene/color4.hpp"
namespace cg
{

/**
 * Presentation node. Holds material properties.
 */
class PresentationNode : public SceneNode
{
  public:
    /**
     * Constructor
     */
    PresentationNode();
    
    /**
     * Constructor with color
     * @param color Color for this presentation node
     */
    PresentationNode(const Color4& color);

    /**
     * Set the color for this presentation node
     * @param color Color to set
     */
    void set_color(const Color4& color);
    
    /**
     * Get the current color
     * @return Current color
     */
    const Color4& get_color() const;
    
    /**
     * Enable or disable blending for transparency
     * @param enable True to enable blending, false to disable
     */
    void set_blending_enabled(bool enable);
    
    /**
     * Check if blending is enabled
     * @return True if blending is enabled
     */
    bool is_blending_enabled() const;
    
    /**
     * Set the blend function parameters
     * @param src_factor Source blend factor (GL_SRC_ALPHA, etc.)
     * @param dst_factor Destination blend factor (GL_ONE_MINUS_SRC_ALPHA, etc.)
     */
    void set_blend_function(GLenum src_factor, GLenum dst_factor);

    
    /**
     * Draw. Sets the material properties.
     * @param  scene_state  Scene state (holds material uniform locations)
     */
    void draw(SceneState &scene_state) override;
  private:
    Color4 color_;              // Current color
    bool blending_enabled_;     // Whether blending is enabled
    GLenum src_blend_factor_;   // Source blend factor
    GLenum dst_blend_factor_;   // Destination blend factor
    
    // Store previous OpenGL state to restore after drawing children
    bool previous_blend_state_;
    GLenum previous_src_factor_;
    GLenum previous_dst_factor_;
};

} // namespace cg

#endif
