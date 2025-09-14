#ifndef __SCENE_LINE_SHADER_NODE_HPP__
#define __SCENE_LINE_SHADER_NODE_HPP__

#include "scene/shader_node.hpp"

namespace cg
{

class LineShaderNode : public ShaderNode
{
public:
    /**
     * Constructor
     */
    LineShaderNode();

    /**
     * Destructor
     */
    virtual ~LineShaderNode();

    /**
     * Create the shader program by loading the basic vertex and fragment shaders.
     * These shaders support both position and color vertex attributes.
     * @return Returns true if successful, false if compile or link errors occur.
     */
    bool create();

    /**
     * Get uniform and attribute locations from the compiled shader program.
     * This method is called after the shader program is successfully linked.
     * @return Returns true if all required locations are found.
     */
    virtual bool get_locations() override;

    /**
     * Draw method - activates the shader, sets up uniforms, and configures line width.
     * @param scene_state Current scene state containing matrices and uniform locations
     */
    virtual void draw(SceneState& scene_state) override;

private:
    GLint color_attr_loc_;  // Location of the color vertex attribute
};

} // namespace cg

#endif // __SCENE_LINE_SHADER_NODE_HPP__
