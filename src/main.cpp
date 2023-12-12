#include <stdlib.h>
#include <iostream>

#include "anthrax.hpp"
#include "Player/player.hpp"


int main()
{
  Anthrax::Anthrax *anthrax_handle_ = new Anthrax::Anthrax();
  int num_cubes_to_render = 10000;
  int x = 100;
  for (int i = 0; i < x; i++)
  {
    for (int j = 0; j < num_cubes_to_render/x; j++)
    {
      Anthrax::vec4<float> color((float)i/x, (float)0.0, (float)j/(num_cubes_to_render/x), (float)1.0);
      Anthrax::vec3<int> pos(i-(x/2), -10, j-(x/2));
      anthrax_handle_->addVoxel(Anthrax::Cube(color, pos));
    }
  }

  Player player = Player(anthrax_handle_);

  try
  {
    anthrax_handle_->startWindow();
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
    window_closed = anthrax_handle_->renderFrame();
    player.processInput();
    player.update();
  }
#endif

  delete(anthrax_handle_);
  return 0;
}
