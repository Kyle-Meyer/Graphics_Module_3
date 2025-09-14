//============================================================================
//	Johns Hopkins University Whiting School of Engineering
//	605.667  Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    Module3/main.cpp
//	Purpose: OpenGL shader program to draw points or lines based on mouse
//           clicks.
//
//============================================================================

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_video.h"
#include "filesystem_support/file_locator.hpp"
#include "geometry/geometry.hpp"
#include "geometry/point2.hpp"
#include "scene/color4.hpp"
#include "scene/graphics.hpp"
#include "scene/presentation_node.hpp"
#include "scene/scene.hpp"

#include <GL/gl.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <geometry/matrix.hpp>
#include "basic_shader_node.hpp"
#include "ngon_geometry_node.hpp"

namespace cg
{

// Simple logging function, should be defined in the cg namespace
void logmsg(const char *message, ...)
{
    // Open file if not already opened
    static FILE *lfile = NULL;
    if(lfile == NULL) { lfile = fopen("Module3.log", "w"); }

    va_list arg;
    va_start(arg, message);
    vfprintf(lfile, message, arg);
    putc('\n', lfile);
    fflush(lfile);
    va_end(arg);
}

} // namespace cg

// SDL Objects
SDL_Window       *g_sdl_window = nullptr;
SDL_GLContext     g_gl_context;
constexpr int32_t DRAWS_PER_SECOND = 30;
constexpr int32_t DRAW_INTERVAL_MILLIS =
    static_cast<int32_t>(1000.0 / static_cast<double>(DRAWS_PER_SECOND));

// Root of the scene graph
std::shared_ptr<cg::SceneNode> g_scene_root;

// Scene state
cg::SceneState g_scene_state;

// Sleep function to help run a reasonable timer
void sleep(int32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

/**
 * Reshape callback. Load a 2-D orthographic projection matrix. Use a world
 * window with width or height of 10 units along the smallest of the screen
 * window dimensions. Set a viewport to draw into the entire window.
 * Note that this callback will be called when the window is opened.
 * @param  width  Window width
 * @param  height Window height
 */
void reshape(int32_t width, int32_t height)
{

    std::cout << "=== RESHAPE CALLED ===" << std::endl;
    glViewport(0, 0, width, height);
    cg::check_error("glViewport");
    
    float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    std::cout << "Window: " << width << "x" << height << ", aspect ratio: " << aspect_ratio << std::endl;

    float world_width, world_height;

    // Determine world window dimensions
    if(width <= height)
    {
        world_width = 10.0f;
        world_height = 10.0f / aspect_ratio;
    }
    else 
    {
        world_height = 10.0f;
        world_width = 10.0f * aspect_ratio;   
    }

    // Bounds of the orthographic projection
    float left = -world_width / 2.0f;
    float right = world_width / 2.0f;
    float bottom = -world_height / 2.0f;
    float top = world_height / 2.0f; 
    float near_plane = -1.0f;
    float far_plane = 1.0f;

    std::cout << "World dimensions: " << world_width << "x" << world_height << std::endl;
    std::cout << "Projection bounds: left=" << left << ", right=" << right 
              << ", bottom=" << bottom << ", top=" << top << std::endl;

    // Calculate orthographic matrix components
    float width_range = right - left;
    float height_range = top - bottom;
    float depth_range = far_plane - near_plane;
    
    std::cout << "Ranges: width=" << width_range << ", height=" << height_range 
              << ", depth=" << depth_range << std::endl;

    // Fill the array with zeros first
    std::fill(g_scene_state.ortho.begin(), g_scene_state.ortho.end(), 0.0f);

    // Set the orthographic projection values (column-major order for OpenGL)
    g_scene_state.ortho[0] = 2.0f / width_range;                              // m[0][0] - X scaling
    g_scene_state.ortho[5] = 2.0f / height_range;                             // m[1][1] - Y scaling  
    g_scene_state.ortho[10] = -2.0f / depth_range;                            // m[2][2] - Z scaling
    g_scene_state.ortho[12] = -(right + left) / width_range;                  // m[0][3] - X translation
    g_scene_state.ortho[13] = -(top + bottom) / height_range;                 // m[1][3] - Y translation
    g_scene_state.ortho[14] = -(far_plane + near_plane) / depth_range;        // m[2][3] - Z translation
    g_scene_state.ortho[15] = 1.0f;                                           // m[3][3] - W component

    std::cout << "Matrix components:" << std::endl;
    std::cout << "  Scale X: " << g_scene_state.ortho[0] << std::endl;
    std::cout << "  Scale Y: " << g_scene_state.ortho[5] << std::endl;
    std::cout << "  Scale Z: " << g_scene_state.ortho[10] << std::endl;
    std::cout << "  Trans X: " << g_scene_state.ortho[12] << std::endl;
    std::cout << "  Trans Y: " << g_scene_state.ortho[13] << std::endl;
    std::cout << "  Trans Z: " << g_scene_state.ortho[14] << std::endl;
    
    std::cout << "=== RESHAPE COMPLETE ===" << std::endl;

}

/**
 * Display callback function
 */
void display(void)
{
    static int frame_count = 0;
    if (frame_count < 5) {  // Only print first 5 frames to avoid spam
        std::cout << "Display frame " << frame_count << std::endl;
    }
    frame_count++;

    // Clear the framebuffer
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (frame_count < 5) {
        std::cout << "About to draw scene root" << std::endl;
    }
    
    g_scene_root->draw(g_scene_state);
    cg::check_error("After Draw");
    
    if (frame_count < 5) {
        std::cout << "Finished drawing, swapping buffers" << std::endl;
    }
    
    // Swap buffers
    SDL_GL_SwapWindow(g_sdl_window);
}

/**
 * Window event handler.
 */
bool handle_window_event(const SDL_Event &event)
{
    bool cont_program = true;
    
    if (event.type == SDL_EVENT_WINDOW_RESIZED || 
        event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) 
    {
        int width, height;
        SDL_GetWindowSize(g_sdl_window, &width, &height);
        reshape(width, height);
        std::cout << "Window resized, calling reshape(" << width << ", " << height << ")" << std::endl;
    }

    return cont_program;
}

/**
 * Keyboard event handler.
 */
bool handle_key_event(const SDL_Event &event)
{
    // bool key_down = event.type == SDL_KEYDOWN; // unused in this module
    bool cont_program = true;
    bool upper_case = event.key.mod & SDL_KMOD_SHIFT || event.key.mod & SDL_KMOD_CAPS;

    switch(event.key.key)
    {
        case SDLK_ESCAPE: cont_program = false; break;
        case SDLK_M:
            // STUDENT TODO
            break;
        default: break;
    }

    return cont_program;
}

/**
 * Mouse button handler (called when a mouse button state changes). Starts a
 * new draggable line when the left button is down. When left button up
 * the line is cleared.
 */
void handle_mouse_event(const SDL_Event &event)
{
    float x_pos = static_cast<float>(event.button.x);
    float y_pos = static_cast<float>(event.button.y);

    // STUDENT TODO
}

/**
 * Mouse motion callback (called when mouse button is depressed)
 */
void handle_mouse_motion_event(const SDL_Event &event)
{
    // STUDENT TODO
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
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: cont_program = false;
            break;

            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: cont_program = handle_window_event(e); 
        break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: handle_mouse_event(e);break;

            case SDL_EVENT_MOUSE_MOTION: handle_mouse_motion_event(e); break;

            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: cont_program = handle_key_event(e);break;
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
  std::cout << "Creating scene graph..." << std::endl;
  
  // Create the root scene node
  g_scene_root = std::make_shared<cg::SceneNode>();
  g_scene_root->set_name("Root");
    
  std::shared_ptr<cg::BasicShaderNode> shader_node = std::make_shared<cg::BasicShaderNode>();
  shader_node->set_name("BasicShader");
  if(!shader_node->create())
  {
    std::cerr << "Failed to make shader node" << std::endl;
    return;
  }

  g_scene_root->add_child(shader_node);

  //======= RED CIRCLE CODE ==========
  std::shared_ptr<cg::PresentationNode> red_presentation_node = std::make_shared<cg::PresentationNode>(cg::Color4(0.75f, 0.0f, 0.0f, 1.0f));
  red_presentation_node->set_name("RedPresentation");
  red_presentation_node->set_blending_enabled(false); // no blending  

  //create the circle geometry 
  std::shared_ptr<cg::NGonGeometryNode> circle_geometry = std::make_shared<cg::NGonGeometryNode>(cg::Point2(0.0f, 0.0f), 32, 0.4f);
  circle_geometry->set_name("CircleGeometry");

  if(!circle_geometry->create())
  {
    std::cerr << "Failed to create circle geometry" << std::endl;
    return;
  } 

  //build hierarchy: shader -> red presentation -> circle geometry
  red_presentation_node->add_child(circle_geometry);
  shader_node->add_child(red_presentation_node);

  //======= BLUE HEXAGON CODE ==========
  std::shared_ptr<cg::PresentationNode> blue_presentation = std::make_shared<cg::PresentationNode>(cg::Color4(0.0f, 0.0f, 0.75f, 0.25f));
  blue_presentation->set_name("BluePresentation");
  blue_presentation->set_blending_enabled(true);
  blue_presentation->set_blend_function(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //create the hexagon geometry 
  std::shared_ptr<cg::NGonGeometryNode> hexagon_geometry = std::make_shared<cg::NGonGeometryNode>(cg::Point2(-2.0f, -2.0f), 6, 0.8f);
  hexagon_geometry->set_name("HexagonGeometry");

  if(!hexagon_geometry->create())
  {
    std::cerr << "Failed to create hexagon geometry" << std::endl;
    return;
  }

  blue_presentation->add_child(hexagon_geometry);
  shader_node->add_child(blue_presentation);

  //======= GREEN OCTAGON CODE ==========
  std::shared_ptr<cg::PresentationNode> green_presentation = std::make_shared<cg::PresentationNode>(cg::Color4(0.0f, 0.75f, 0.0f, 0.5f));
  green_presentation->set_name("GreenPresentation");
  green_presentation->set_blending_enabled(true);
  green_presentation->set_blend_function(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  std::shared_ptr<cg::NGonGeometryNode> octagon_geometry = std::make_shared<cg::NGonGeometryNode>(cg::Point2(2.5f, 2.5f), 8, 0.6f);
  octagon_geometry->set_name("OctagonGeometry");

  if(!octagon_geometry->create())
  {
    std::cerr << "Failed to create octagon geometry" << std::endl;
    return;
  }

  green_presentation->add_child(octagon_geometry);
  shader_node->add_child(green_presentation);

  std::cout << "Scene graph created successfully!" << std::endl;
  std::cout << "Objects created:" << std::endl;
  std::cout << "  - Red circle (32-gon) at (0,0), radius 4.5, opaque" << std::endl;
  std::cout << "  - Blue hexagon at (-2,-2), radius 3, 25% opaque" << std::endl;
  std::cout << "  - Green octagon at (2.5,2.5), radius 2, 50% transparent" << std::endl;
 
  // Print the complete scene graph structure
  std::cout << "\nScene graph structure:" << std::endl;
  g_scene_root->print_graph(std::cout, 0);
  
  cg::check_error("create_scene");
}

bool init_sdl()
{
  if(!SDL_InitSubSystem(SDL_INIT_VIDEO))
  {
    std::cerr << "Failed to init SDL sub system: Video " << SDL_GetError() << std::endl;
    return false;
  }

  std::cout << "initialized SDL video!" << std::endl;
  return true;
}

bool init_display_mode()
{
  //set openGL attributes
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  //enable double buffers
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  //true color 
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
  
  // Enable multi-sample anti-aliasing (MSAA)
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);  // 4x MSAA
  
  //create window with size and name 
  g_sdl_window =  SDL_CreateWindow(
    "Kyle Meyer (remembered this time :])",
    800, 800,
    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
  );

  if(!g_sdl_window)
  {
    std::cerr << "could not create SDL WINDOW! " << SDL_GetError() << std::endl;
    return false;
  }

  SDL_SetWindowPosition(g_sdl_window, 200, 200);
  std::cout << "created window successfully" << std::endl;
  return true;
}

bool init_openGL()
{
  g_gl_context = SDL_GL_CreateContext(g_sdl_window);
  if(!g_gl_context)
  {
    std::cerr << "could not create gl context: " << SDL_GetError() << std::endl;
    return false;
  }

  if(SDL_GL_SetSwapInterval(1) != 0)
  {
    std::cout << "Vsync is NOT on" << std::endl;
  }

  //black
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  
  glEnable(GL_MULTISAMPLE);

  // Display OpenGL information
  std::cout << "OpenGL context created successfully" << std::endl;
  std::cout << "OpenGL  " << glGetString(GL_VERSION) << ", GLSL "
            << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  
  // Check if we got the MSAA we requested
  int msaa_buffers, msaa_samples;
  SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &msaa_buffers);
  SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &msaa_samples);
  std::cout << "MSAA: " << msaa_buffers << " buffers, " << msaa_samples << " samples" << std::endl;
  
  return true;
}

// Cleanup function
void cleanup_sdl_opengl()
{
  if (g_gl_context) {
      SDL_GL_DestroyContext(g_gl_context);
      g_gl_context = nullptr;
  }
  if (g_sdl_window) {
      SDL_DestroyWindow(g_sdl_window);
      g_sdl_window = nullptr;
  }
  SDL_Quit();
  std::cout << "SDL and OpenGL cleaned up successfully" << std::endl;
}

/**
 * Main - entry point for GetStarted GLUT application.
 */
int main(int argc, char **argv)
{
    cg::set_root_paths(argv[0]);
    std::cout << "Keyboard Controls:\n";
    std::cout << "M : Enable MSAA    m : Disable MSAA\n";
    std::cout << "ESC - Exit program\n";

    // Initialize SDL
  if (!(init_sdl())) 
  {
      return -1;
  }

    // Initialize display mode and window
  if (!init_display_mode()) 
  {
        SDL_Quit();
        return -1;
  }

  // Initialize OpenGL
  if (!init_openGL()) 
  {
        cleanup_sdl_opengl();
        return -1;
    }

    std::cout << "OpenGL  " << glGetString(GL_VERSION) << ", GLSL "
              << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

    // Initialize GLEW (if using Windows)
    // STUDENT TODO

    float aliased_line_width_range[2];
    float point_size_range[2];

    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, aliased_line_width_range);
    glGetFloatv(GL_POINT_SIZE_RANGE, point_size_range);

    std::cout << "GL_ALIASED_LINE_WIDTH_RANGE: " << aliased_line_width_range[0] << ", "
              << aliased_line_width_range[1] << '\n';
    std::cout << "GL_POINT_SIZE_RANGE: " << point_size_range[0] << ", " << point_size_range[1]
              << '\n';

#if defined(GL_POINT_SPRITE)
    // NOTE: Some windowing systems contain a bug that requires the following line of code to
    // execute BEFORE vertex shaders will populate 'gl_PointCoord'
    glEnable(GL_POINT_SPRITE);
#endif
   
    // Set initial viewport and projection matrix
    int initial_width, initial_height;
    SDL_GetWindowSize(g_sdl_window, &initial_width, &initial_height);
    reshape(initial_width, initial_height);
    std::cout << "Initial reshape called with: " << initial_width << "x" << initial_height << std::endl;
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
    cleanup_sdl_opengl();
    return 0;
}
