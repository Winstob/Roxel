#include <stdlib.h>
#include <iostream>

#include "anthrax.hpp"
#include "Player/player.hpp"


int main()
{
  Anthrax::Anthrax *anthrax_handle_ = new Anthrax::Anthrax();
  int num_cubes_to_render = 800;
  int x = floor(sqrt(num_cubes_to_render));
  num_cubes_to_render = x*x;
  for (int i = 0; i < x; i++)
  {
    for (int j = 0; j < num_cubes_to_render/x; j++)
    {
      Anthrax::vec4<float> color((float)i/x, (float)0.0, (float)j/(num_cubes_to_render/x), (float)1.0);
      Anthrax::vec3<int> pos(i-(x/2), -1000, j-(x/2));
      anthrax_handle_->addVoxel(Anthrax::Cube(color, pos));
    }
  }

  Player player = Player(anthrax_handle_);
  player.updateForce("gravity", Anthrax::vec3<float>(0.0, -9.8, 0.0));

  try
  {
    anthrax_handle_->startWindow();
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
  }
  bool window_closed = false;

#ifndef WIN32
  time_t time_frame_start;
  time_t time_now;
  time(&time_frame_start);
  int num_frames = 0;
#endif
  while (!window_closed)
  {
#ifndef WIN32
    time(&time_now);
    if (time_now-time_frame_start >= 1)
    {
      time(&time_frame_start);
      std::cout << "Framerate: " << num_frames << " FPS" << std::endl;
      num_frames = 0;
    }
    num_frames++;
#endif
    window_closed = anthrax_handle_->renderFrame();
    player.processInput();
    player.update();
  }

  delete(anthrax_handle_);
  return 0;
}
