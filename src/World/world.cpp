/* ---------------------------------------------------------------- *\
 * world.cpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-12
\* ---------------------------------------------------------------- */
#include "world.hpp"

#include <iostream>

World::World(std::string directory)
{
  directory_ = directory;
  octree_ = Octree(num_layers_, zone_depth_);
}


void World::loadArea(Anthrax::vec3<int> center, int load_distance)
{
  octree_.loadArea(center, load_distance);
}
