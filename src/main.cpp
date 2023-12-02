#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdlib.h>

#include "window.cpp"

#include <iostream>

int main()
{
  Engine::num_cubes_to_render = 10000;
  int x = 100;
  Engine::cubes_to_render = (Cube*)malloc(Engine::num_cubes_to_render*sizeof(Cube));
  Engine::cubes_to_render[0] = Cube({0.5, 0.0, 0.0, 1.0}, {0, 0, 0});
  Engine::cubes_to_render[1] = Cube({0.0, 0.0, 0.5, 1.0}, {0, 1, 0});
  int k = 0;
  for (int i = 0; i < x; i++)
  {
    for (int j = 0; j < Engine::num_cubes_to_render/x; j++)
    {
      Engine::cubes_to_render[k] = Cube({(float)Engine::num_cubes_to_render/(i+1), 0.0, (float)i/Engine::num_cubes_to_render, 1.0}, {i, j, 0});
      k++;
    }
  }

  try
  {
    Engine::startWindow();
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
  }
  bool window_closed = false;

#ifdef WIN32
  while (!window_closed)
  {
    window_closed = Engine::renderFrame();
  }
#else
  time_t time_frame_start;
  time_t time_now;
  time(&time_frame_start);
  int num_frames = 0;
  while (!window_closed)
  {
    time(&time_now);
    if (time_now-time_frame_start >= 1)
    {
      time(&time_frame_start);
      std::cout << "Framerate: " << num_frames << " FPS" << std::endl;
      num_frames = 0;
    }
    num_frames++;
    window_closed = Engine::renderFrame();
  }
#endif

  delete(Engine::cubes_to_render);
  return 0;
}
