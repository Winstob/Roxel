#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600


class Window
{
public:
  Window();
  void init();
  void run();
private:
  GLFWwindow* window_;
  static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
};
