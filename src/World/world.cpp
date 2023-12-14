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
  octree_ = Octree(num_layers_, zone_depth_, directory_ + "/", Anthrax::vec3<int64_t>(0, 0, 0));
}


void World::loadArea(Anthrax::vec3<int64_t> center, int load_distance)
{
  octree_.loadArea(center, load_distance);
}


void World::getCubes(std::vector<Anthrax::Cube> *cube_vector)
{
  octree_.getCubes(cube_vector);
}
