/* ---------------------------------------------------------------- *\
 * world.cpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-12
\* ---------------------------------------------------------------- */
#include "world.hpp"
#include "cubeconvert.hpp"

#include <iostream>

// Allocate space for static member variables
CubeConvert Octree::cube_converter_;
Anthrax::Anthrax *Octree::anthrax_instance_;
bool (*Octree::loadDecisionFunction)(uint64_t, int);

World::World(std::string directory, Anthrax::Anthrax *anthrax_instance)
{
  directory_ = directory;
  octree_ = Octree(num_layers_, zone_depth_, directory_ + "/", Anthrax::vec3<int64_t>(0, 0, 0));
  octree_.setCubeSettingsFile("voxelmap.json");

  anthrax_instance_ = anthrax_instance;
  octree_.setAnthraxPointer(anthrax_instance_);
  octree_.setLoadDecisionFunction([](uint64_t distance, int layer) {
      return (distance < 5000 && layer > distance / 100);
      });
}


void World::loadArea(Anthrax::vec3<int64_t> center)
{
  octree_.loadArea(center);
  octree_.getNewNeighbors();
}


void World::getCubes()
{
  octree_.getCubes();
}
