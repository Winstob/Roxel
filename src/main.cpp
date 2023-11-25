#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "window.cpp"

#include <iostream>

int main()
{
  try
  {
    startWindow();
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
