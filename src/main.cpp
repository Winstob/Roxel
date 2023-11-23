#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "window.hpp"

#include <iostream>

int main()
{
  Window *window = new Window();
  try
  {
    window->init();
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
  }
  window->run();
  delete(window);
  return 0;
}
