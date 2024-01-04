/* ---------------------------------------------------------------- *\
 * anthrax.cpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-10
\* ---------------------------------------------------------------- */

#include <iostream>
#include "anthrax.hpp"

namespace Anthrax
{

// Initialize staic member variables
GLFWwindow* Anthrax::window;
Camera Anthrax::camera;
unsigned int Anthrax::window_width_;
unsigned int Anthrax::window_height_;
unsigned int Anthrax::render_distance_;
float Anthrax::mouse_x_;
float Anthrax::mouse_y_;
float Anthrax::deltaTime;
float Anthrax::lastFrame;
bool Anthrax::wireframe_mode_;


Anthrax::Anthrax()
{
  window = NULL;
  camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));
  window_width_ = 800;
  window_height_ = 600;
  render_distance_ = 10000;
  mouse_x_ = window_width_ / 2.0f;
  mouse_y_ = window_height_ / 2.0f;
  deltaTime = 0.0f;
  lastFrame = 0.0f;
  wireframe_mode_ = false;
}

int Anthrax::startWindow()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  window = glfwCreateWindow(window_width_, window_height_, "Roxel", NULL, NULL);
  //window = glfwCreateWindow(window_width_, window_height_, "Roxel", glfwGetPrimaryMonitor(), NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetKeyCallback(window, key_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile our shader zprogram
  // ------------------------------------
  cube_shader = new Shader(Shader::ShaderInputType::CODESTRING, cube_vertex_shader.c_str(), cube_fragment_shader.c_str(), cube_geometry_shader.c_str());

  // Initialize the voxel cache
  voxel_cache_manager_ = new VoxelCacheManager();
  voxel_cache_manager_->initialize(MB(16));

  // Face culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  return 1;
}


int Anthrax::renderFrame()
{
  // render loop
  // -----------
  // per-frame time logic
  // --------------------
  float currentFrame = static_cast<float>(glfwGetTime());
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  // input
  // -----
  processInput(window);

  // render
  // ------
  //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

  renderScene();

  // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
  // -------------------------------------------------------------------------------
  glfwSwapBuffers(window);
  glfwPollEvents();

  if (glfwWindowShouldClose(window))
  {
    delete cube_shader;

    delete voxel_cache_manager_;
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 1;
  }
  return 0;
}


void Anthrax::renderScene()
{
  // Set up shader
  cube_shader->use();

  // Camera/view settings
  // Set camera position
  cube_shader->setVec3("view_position", camera.position_);
  // Camera/view transformation
  glm::mat4 view = camera.GetViewMatrix();
  cube_shader->setMat4("view", view);
  // Pass projection matrix to shader (note that in this case it could change every frame)
  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window_width_ / (float)window_height_, 0.1f, (float)render_distance_);
  cube_shader->setMat4("projection", projection);

  // Set sunlight settings
  //cube_shader->setVec3("sunlight.direction", glm::vec3(-1.0f, -0.5f, 0.0f));
  cube_shader->setVec3("sunlight.direction", glm::vec3(glm::cos(glfwGetTime()/16), glm::sin(glfwGetTime()/16), 0.0f));
  cube_shader->setVec3("sunlight.ambient", glm::vec3(0.8));
  cube_shader->setVec3("sunlight.diffuse", glm::vec3(1.0));
  cube_shader->setVec3("sunlight.specular", glm::vec3(0.3));

  if (voxel_buffer_.size() > 0)
  {
    voxel_cache_manager_->addCubes(&(voxel_buffer_[0]), voxel_buffer_.size());
    voxel_buffer_.clear();
  }
  voxel_cache_manager_->renderCubes();
}

void Anthrax::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_0 && action  == GLFW_PRESS)
  {
    if (wireframe_mode_)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      wireframe_mode_ = false;
    }
    else
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      wireframe_mode_ = true;
    }
  }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Anthrax::processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Anthrax::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
  window_width_ = width;
  window_height_ = height;
  mouse_x_ = window_width_ / 2.0f;
  mouse_y_ = window_height_ / 2.0f;
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Anthrax::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  mouse_x_ = xpos;
  mouse_y_ = ypos;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Anthrax::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

bool Anthrax::getKeyPress(Key key)
{
  switch(key)
  {
    case Key::A:
      return (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
    case Key::D:
      return (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
    case Key::S:
      return (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
    case Key::W:
      return (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
    case Key::SPACE:
      return (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    case Key::LSHIFT:
      return (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    case Key::LCTRL:
      return (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
    default:
      return false;
  }
}

void Anthrax::getMousePosition(MousePosition *mouse_pos)
{
  mouse_pos->setX(mouse_x_);
  mouse_pos->setY(mouse_y_);
}

void Anthrax::addVoxel(Cube cube)
{
  voxel_buffer_.push_back(cube);
}

void Anthrax::setCameraPosition(vec3<float> position)
{
  camera.setPosition(glm::vec3(position.getX(), position.getY(), position.getZ()));
}

void Anthrax::setCameraRotation(Quaternion rotation)
{
  camera.setRotation(glm::quat(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ()));
}

} // namespace Anthrax
