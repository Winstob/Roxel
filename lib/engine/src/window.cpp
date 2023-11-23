#include "window.hpp"

Window::Window()
{
}

void Window::init()
{
  // Initialize GLFW
  if (!glfwInit())
  {
    throw(std::runtime_error("Failed to initialize GLFW"));
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create the glfw window object
  int window_width = DEFAULT_WINDOW_WIDTH;
  int window_height = DEFAULT_WINDOW_HEIGHT;
  window_ = glfwCreateWindow(window_width, window_height, "Roxel", NULL, NULL);
  if (window_ == NULL)
  {
    glfwTerminate();
    throw(std::runtime_error("Failed to initialize GLFW window"));
  }
  glfwMakeContextCurrent(window_);

  // Initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    throw(std::runtime_error("Failed to initialize GLAD"));
  }

  // Set callbacks
  glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
}


void Window::run()
{
  while (!glfwWindowShouldClose(window_))
  {
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
  glfwTerminate();
}


/* -------------------------------------------------- *\
 * Callbacks
\* -------------------------------------------------- */

void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}


