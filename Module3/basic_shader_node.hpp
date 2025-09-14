#ifndef __SCENE_BASIC_SHADER_NODE_HPP__
#define __SCENE_BASIC_SHADER_NODE_HPP__

#include "scene/shader_node.hpp"

namespace cg
{

/**
 * Basic shader node for 2D rendering with solid colors and orthographic projection.
 * This concrete implementation of ShaderNode loads vertex and fragment shaders
 * from external files for rendering 2D geometry with uniform colors.
 */
class BasicShaderNode : public ShaderNode
{
public:
    /**
     * Constructor
     */
    BasicShaderNode();

    /**
     * Destructor
     */
    virtual ~BasicShaderNode();

    /**
     * Create the shader program by loading vertex and fragment shaders from files.
     * Looks for "basic.vert" and "basic.frag" in the Module3 directory.
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
     * Draw method - activates the shader and sets up uniforms.
     * @param scene_state Current scene state containing matrices and uniform locations
     */
    virtual void draw(SceneState& scene_state) override;
};

} // namespace cg

#endif // __SCENE_BASIC_SHADER_NODE_HPP__
