#include "scene/presentation_node.hpp"
#include "scene/scene.hpp"

namespace cg
{

PresentationNode::PresentationNode() 
    : color_(1.0f, 1.0f, 1.0f, 1.0f),  // Default to opaque white
      blending_enabled_(false),
      src_blend_factor_(GL_SRC_ALPHA),
      dst_blend_factor_(GL_ONE_MINUS_SRC_ALPHA),
      previous_blend_state_(false),
      previous_src_factor_(GL_SRC_ALPHA),
      previous_dst_factor_(GL_ONE_MINUS_SRC_ALPHA)
{ 
    node_type_ = SceneNodeType::PRESENTATION; 
}

PresentationNode::PresentationNode(const Color4& color)
    : color_(color),
      blending_enabled_(color.a < 1.0f),  // Enable blending if not fully opaque
      src_blend_factor_(GL_SRC_ALPHA),
      dst_blend_factor_(GL_ONE_MINUS_SRC_ALPHA),
      previous_blend_state_(false),
      previous_src_factor_(GL_SRC_ALPHA),
      previous_dst_factor_(GL_ONE_MINUS_SRC_ALPHA)
{
    node_type_ = SceneNodeType::PRESENTATION;
}

void PresentationNode::set_color(const Color4& color)
{
    color_ = color;
    // Automatically enable blending if alpha is less than 1.0
    if (color_.a < 1.0f && !blending_enabled_)
    {
        blending_enabled_ = true;
    }
}

const Color4& PresentationNode::get_color() const
{
    return color_;
}

void PresentationNode::set_blending_enabled(bool enable)
{
    blending_enabled_ = enable;
}

bool PresentationNode::is_blending_enabled() const
{
    return blending_enabled_;
}

void PresentationNode::set_blend_function(GLenum src_factor, GLenum dst_factor)
{
    src_blend_factor_ = src_factor;
    dst_blend_factor_ = dst_factor;
}

void PresentationNode::draw(SceneState& scene_state)
{
    // Store current OpenGL blend state
    previous_blend_state_ = glIsEnabled(GL_BLEND) == GL_TRUE;
    if (previous_blend_state_)
    {
        glGetIntegerv(GL_BLEND_SRC_ALPHA, reinterpret_cast<GLint*>(&previous_src_factor_));
        glGetIntegerv(GL_BLEND_DST_ALPHA, reinterpret_cast<GLint*>(&previous_dst_factor_));
    }
    
    // Set up blending if enabled
    if (blending_enabled_)
    {
        glEnable(GL_BLEND);
        glBlendFunc(src_blend_factor_, dst_blend_factor_);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    
    // Set the color uniform if we have a valid location
    if (scene_state.color_loc != -1)
    {
        glUniform4f(scene_state.color_loc, color_.r, color_.g, color_.b, color_.a);
        cg::check_error("PresentationNode::draw - setting color uniform");
    }
    
    // Draw all children with the current presentation state
    SceneNode::draw(scene_state);
    
    // Restore previous blend state
    if (previous_blend_state_)
    {
        glEnable(GL_BLEND);
        glBlendFunc(previous_src_factor_, previous_dst_factor_);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    
    cg::check_error("PresentationNode::draw - end");
}

} // namespace cg
