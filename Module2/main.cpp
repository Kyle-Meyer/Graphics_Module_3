//============================================================================
//	Johns Hopkins University Whiting School of Engineering
//	605.667  Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    Module2/main.cpp
//	Purpose: OpenGL shader program to draw points or lines based on mouse
//           clicks.
//
//============================================================================

#include "filesystem_support/file_locator.hpp"
#include "geometry/geometry.hpp"
#include "scene/graphics.hpp"
#include "scene/scene.hpp"
#include "shader_support/glsl_shader.hpp"

#include "Module2/line_node.hpp"
#include "Module2/line_shader_node.hpp"
#include "Module2/point_node.hpp"
#include "Module2/point_shader_node.hpp"
#include "Module2/shader_src.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace cg
{

// Simple logging function, should be defined in the cg namespace
void logmsg(const char *message, ...)
{
    // Open file if not already opened
    static FILE *lfile = NULL;
    if(lfile == NULL) { lfile = fopen("Module2.log", "w"); }

    va_list arg;
    va_start(arg, message);
    vfprintf(lfile, message, arg);
    putc('\n', lfile);
    fflush(lfile);
    va_end(arg);
}

} // namespace cg

// Root of the scene graph
std::shared_ptr<cg::SceneNode> g_scene_root;

// Scene state
cg::SceneState g_scene_state;

// PointNode - global so we can add points dynamically
std::shared_ptr<cg::PointNode> g_points;

// PointShaderNode - global so we can get the point location
std::shared_ptr<cg::PointShaderNode> g_point_shader;

// LineNode
std::shared_ptr<cg::LineNode> g_lines;

// LineShaderNode - global so we can get the point location
std::shared_ptr<cg::LineShaderNode> g_line_shader;

// SDL Objects
SDL_Window       *g_sdl_window = nullptr;
SDL_GLContext     g_gl_context;
constexpr int32_t DRAWS_PER_SECOND = 30;
constexpr int32_t DRAW_INTERVAL_MILLIS =
    static_cast<int32_t>(1000.0 / static_cast<double>(DRAWS_PER_SECOND));

// Sleep function to help run a reasonable timer
void sleep(int32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

/**
 * Reshape callback. Load a 2-D orthographic projection matrix using the
 * window width and height so we can directly take window coordinates and
 * send to OpenGL. Note that this callback will be called when the window
 * is opened.
 * @param  width  Window width
 * @param  height Window height
 */
void reshape(int32_t width, int32_t height)
{
    // Set a simple 4x4 matrix (use an array until we develop the Matrix4x4 class)
    g_scene_state.ortho[0] = 2.0f / static_cast<float>(width);
    g_scene_state.ortho[1] = 0.0f;
    g_scene_state.ortho[2] = 0.0f;
    g_scene_state.ortho[3] = 0.0f;
    g_scene_state.ortho[4] = 0.0f;
    g_scene_state.ortho[5] = -2.0f / static_cast<float>(height);
    g_scene_state.ortho[6] = 0.0f;
    g_scene_state.ortho[7] = 0.0f;
    g_scene_state.ortho[8] = 0.0f;
    g_scene_state.ortho[9] = 0.0f;
    g_scene_state.ortho[10] = 1.0f;
    g_scene_state.ortho[11] = 0.0f;
    g_scene_state.ortho[12] = -1.0f;
    g_scene_state.ortho[13] = 1.0f;
    g_scene_state.ortho[14] = 0.0f;
    g_scene_state.ortho[15] = 1.0f;

    // Update the viewport
    glViewport(0, 0, width, height);
}

/**
 * Display function
 */
void display()
{
    // Clear the framebuffer
    glClear(GL_COLOR_BUFFER_BIT);

    g_scene_root->draw(g_scene_state);
    cg::check_error("After Draw");

    // Swap buffers
    SDL_GL_SwapWindow(g_sdl_window);
}

/**
 * Window event handler.
 */
bool handle_window_event(const SDL_Event &event)
{
    bool cont_program = true;

    switch(event.type)
    {
        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            reshape(event.window.data1, event.window.data2);
            break;
        default: break;
    }

    return cont_program;
}

/**
 * Mouse event handler.
 */
bool handle_mouse_event(const SDL_Event &event)
{
    float x_pos, y_pos;
    switch(event.button.button)
    {
        case 1: // Left Mouse Button
            x_pos = static_cast<float>(event.button.x);
            y_pos = static_cast<float>(event.button.y);
            // Add a point to display
            g_points->add(x_pos, y_pos, g_point_shader->get_position_loc());

            // Add a point to the polyline
            g_lines->add(x_pos, y_pos, g_line_shader->get_position_loc());
            break;
        default: break;
    }

    return true;
}

/**
 * Keyboard event handler.
 */
bool handle_key_event(const SDL_Event &event)
{
    bool cont_program = true;

    switch(event.key.key)
    {
        case SDLK_ESCAPE: cont_program = false; break;
        case SDLK_SPACE: break;
        case SDLK_M: break;
        case SDLK_1:
            glLineWidth(1.0f);
            glPointSize(2.0f);
            break;
        case SDLK_2:
            glLineWidth(2.0f);
            glPointSize(4.0f);
            break;
        case SDLK_3:
            glLineWidth(3.0f);
            glPointSize(6.0f);
            break;
        case SDLK_4:
            glLineWidth(4.0f);
            glPointSize(8.0f);
            break;
        case SDLK_5:
            glLineWidth(5.0f);
            glPointSize(10.0f);
            break;
        case SDLK_6:
            glLineWidth(6.0f);
            glPointSize(12.0f);
            break;
        case SDLK_7:
            glLineWidth(7.0f);
            glPointSize(14.0f);
            break;
        case SDLK_8:
            glLineWidth(8.0f);
            glPointSize(16.0f);
            break;
        case SDLK_9:
            glLineWidth(9.0f);
            glPointSize(18.0f);
            break;
        default: break;
    }

    return cont_program;
}

/**
 * Handle Events function.
 */
bool handle_events()
{
    SDL_Event e;
    bool      cont_program = true;
    while(SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_EVENT_QUIT:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: cont_program = false; break;

            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: cont_program = handle_window_event(e); break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: cont_program = handle_mouse_event(e); break;

            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: cont_program = handle_key_event(e); break;
            default: break;
        }
    }
    return cont_program;
}

/**
 * Create the scene.
 */
void create_scene()
{
    // Create a scene graph with 2 shaders (one for offset lines and one
    // for points) and 2 geometry nodes
    g_line_shader = std::make_shared<cg::LineShaderNode>();
    if(!g_line_shader->create("Module2/lines.vert", "Module2/lines.frag") ||
       !g_line_shader->get_locations())
    // if(!g_line_shader->create_from_source(lines_vert, lines_frag) ||
    // !g_line_shader->get_locations())
    {
        exit(-1);
    }

    g_lines = std::make_shared<cg::LineNode>(cg::Color4(0.1f, 0.1f, 6.1f, 1.0f));

    // Create the point shader node
    g_point_shader = std::make_shared<cg::PointShaderNode>();
    if(!g_point_shader->create("Module2/points.vert", "Module2/points.frag") ||
       !g_point_shader->get_locations())
    // if(!g_point_shader->create_from_source(points_vert, points_frag) ||
    // !g_point_shader->get_locations())
    {
        exit(-1);
    }

    // Create the node that manages the points
    g_points = std::make_shared<cg::PointNode>();

    // Create scene graph
    g_scene_root = std::make_shared<cg::SceneNode>();
    g_scene_root->add_child(g_line_shader);
    g_line_shader->add_child(g_lines);
    g_scene_root->add_child(g_point_shader);
    g_point_shader->add_child(g_points);
}

/**
 * Main - entry point for GetStarted GLUT application.
 */
int main(int argc, char **argv)
{
    cg::set_root_paths(argv[0]);
    std::cout << "Keyboard Controls:\n";
    std::cout << "1-9 : Alter line width and point size\n";
    std::cout << "ESC - Exit program\n";

    // Initialize SDL
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "Error initializing SDL: " << SDL_GetError() << '\n';
        exit(1);
    }

    // Initialize display mode and window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_PropertiesID props = SDL_CreateProperties();
    if(props == 0)
    {
        std::cout << "Error creating SDL Window Properties: " << SDL_GetError() << '\n';
        exit(1);
    }

    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Module2 by Brian Russin");
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 800);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 600);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, 200);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, 200);

    g_sdl_window = SDL_CreateWindowWithProperties(props);
    if(g_sdl_window == nullptr)
    {
        std::cout << "Error initializing SDL Window" << SDL_GetError() << '\n';
        exit(1);
    }

    // Initialize OpenGL
    g_gl_context = SDL_GL_CreateContext(g_sdl_window);

    if(g_gl_context == nullptr)
    {
        std::cout << "Error Getting OpenGL Context: " << SDL_GetError() << '\n';
        exit(1);
    }

    std::cout << "OpenGL  " << glGetString(GL_VERSION) << ", GLSL "
              << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

#if BUILD_WINDOWS
    int32_t glew_init_result = glewInit();
    if(GLEW_OK != glew_init_result)
    {
        std::cout << "GLEW Error: " << glewGetErrorString(glew_init_result) << '\n';
        exit(EXIT_FAILURE);
    }
#endif

    float aliased_line_width_range[2];
    float point_size_range[2];

    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, aliased_line_width_range);
    glGetFloatv(GL_POINT_SIZE_RANGE, point_size_range);

    std::cout << "GL_ALIASED_LINE_WIDTH_RANGE: " << aliased_line_width_range[0] << ", "
              << aliased_line_width_range[1] << '\n';
    std::cout << "GL_POINT_SIZE_RANGE: " << point_size_range[0] << ", " << point_size_range[1]
              << '\n';

    // Set the clear color to white
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    reshape(800, 600);

    // Set a default point size and line width
    glPointSize(8.0f);
    glLineWidth(4.0f);

    // Setting so gl_PointCoord has origin at lower left.
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

#if defined(GL_POINT_SPRITE)
    // NOTE: Some windowing systems contain a bug that requires the following line of code to
    // execute BEFORE vertex shaders will populate 'gl_PointCoord'
    glEnable(GL_POINT_SPRITE);
#endif

    // Create the scene
    create_scene();
    cg::check_error("create_scene");

    // Main loop
    while(handle_events())
    {
        display();
        sleep(DRAW_INTERVAL_MILLIS);
    }

    // Destroy OpenGL Context, SDL Window and SDL
    SDL_GL_DestroyContext(g_gl_context);
    SDL_DestroyWindow(g_sdl_window);
    SDL_Quit();

    return 0;
}
