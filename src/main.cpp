#include <stdlib.h>
#include <iostream>

#include "window.cpp"
#include "Player/player.hpp"


int main()
{
  Anthrax::num_cubes_to_render = 10000;
  int x = 100;
  Anthrax::cubes_to_render = (Anthrax::Cube*)malloc(Anthrax::num_cubes_to_render*sizeof(Anthrax::Cube));
  int k = 0;
  for (int i = 0; i < x; i++)
  {
    for (int j = 0; j < Anthrax::num_cubes_to_render/x; j++)
    {
      Anthrax::vec4<float> color((float)i/x, (float)0.0, (float)j/(Anthrax::num_cubes_to_render/x), (float)1.0);
      Anthrax::vec3<int> pos(i-(x/2), -10, j-(x/2));
      Anthrax::cubes_to_render[k] = Anthrax::Cube(color, pos);
      k++;
    }
  }

  Player player = Player();

  try
  {
    Anthrax::startWindow();
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
  }
  bool window_closed = false;

#ifdef WIN32
  while (!window_closed)
  {
    window_closed = Anthrax::renderFrame();
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
    window_closed = Anthrax::renderFrame();
    player.processInput();
  }
#endif

  delete(Anthrax::cubes_to_render);
  return 0;
}
