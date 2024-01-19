/* ---------------------------------------------------------------- *\
 * main.cpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-02
\* ---------------------------------------------------------------- */
#include <stdlib.h>
#include <iostream>

#include "anthrax.hpp"
#include "Player/player.hpp"
#include "World/world.hpp"


int main()
{
  Anthrax::Anthrax *anthrax_handle_ = new Anthrax::Anthrax();
  try
  {
    anthrax_handle_->startWindow();
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
  }
  bool window_closed = false;


  // Create a container to hold all the voxels that may need to be displayed, hand it to the world manager
  World world = World("world", anthrax_handle_);
  world.loadArea(Anthrax::vec3<int64_t>(0, 0, 0));

  //std::map<uint16_t, std::vector<Anthrax::Cube>> cube_map;
  /*
  world.getCubes(&cube_map);
  anthrax_handle_->voxel_buffer_map_ = cube_map;
  */
  //world.getCubes();

  Player player = Player(anthrax_handle_);
  //player.updateForce("gravity", Anthrax::vec3<float>(0.0, -9.8, 0.0));

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
    Anthrax::vec3<int64_t> position = Anthrax::vec3<int64_t>(player.getPosition().getX(), player.getPosition().getY(), player.getPosition().getZ());
    world.loadArea(position);

    window_closed = anthrax_handle_->renderFrame();
    player.processInput();
    player.update();
  }

  delete anthrax_handle_;
  return 0;
}
