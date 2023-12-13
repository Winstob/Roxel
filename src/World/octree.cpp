/* ---------------------------------------------------------------- *\
 * octree.cpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-12
\* ---------------------------------------------------------------- */
#include "octree.hpp"


Octree::Octree()
{
  Octree(0, 0);
}


Octree::Octree(unsigned int layer, unsigned int file_layer)
{
  Octree(layer, file_layer, "");
}


Octree::Octree(unsigned int layer, unsigned int file_layer, std::string path)
{
  layer_ = layer;
  file_layer_ = file_layer;
  center_ = Anthrax::vec3<int>(0, 0, 0);
  if (layer == 0)
  {
    is_leaf_ = true;
  }
  else
  {
    is_leaf_ = false;
  }
  is_uniform_ = false;
  path_ = path;
}


Octree::~Octree()
{
  for (unsigned int i = 0; i < 8; i++)
  {
    if (children_[i] != NULL)
    {
      delete(children_[i]);
    }
  }
}


void Octree::loadArea(Anthrax::vec3<int> center, int load_distance)
{
  // First, find the quadrant which the desired center resides in
  Anthrax::vec3<int>direction_to_position = center - center_;
  int x = direction_to_position.getX() <= 0 ? 0 : 1;
  int y = direction_to_position.getY() <= 0 ? 0 : 4;
  int z = direction_to_position.getZ() <= 0 ? 0 : 2;
  int quadrant = x + y + z;
  // Iterate through all 8 quadrants to find which ones need to be loaded
  bool load_quadrant[8];
  load_quadrant[quadrant] = true;
  int quadrant_width = (1 << layer_); // 2^layer_
  for (unsigned int i = 0; i < 8; i++)
  {
    if (i == quadrant) continue;
    // Find the closest point to the center of the desired loading area
    int min_x, max_x, min_y, max_y, min_z, max_z;
    if (i%2 == 0)
    {
      min_x = center_.getX() - quadrant_width;
      max_x = center_.getX();
    }
    else
    {
      min_x = center_.getX();
      max_x = center_.getX() + quadrant_width;
    }

    if (i < 4)
    {
      min_y = center_.getY() - quadrant_width;
      max_y = center_.getY();
    }
    else
    {
      min_y = center_.getY();
      max_y = center_.getY() + quadrant_width;
    }
 
    if (i == 0 || i == 1 || i == 4 || i == 5)
    {
      min_z = center_.getZ() - quadrant_width;
      max_z = center_.getZ();
    }
    else
    {
      min_z = center_.getZ();
      max_z = center_.getZ() + quadrant_width;
    }

    // Now solve for the closest point
    if (center.getX() <= min_x)
    {
      x = min_x;
    }
    else if (center.getX() >= max_x)
    {
      x = max_x;
    }
    else
    {
      x = center.getX();
    }

    if (center.getY() <= min_y)
    {
      y = min_y;
    }
    else if (center.getY() >= max_y)
    {
      y = max_y;
    }
    else
    {
      y = center.getY();
    }

    if (center.getZ() <= min_z)
    {
      z = min_z;
    }
    else if (center.getZ() >= max_z)
    {
      z = max_z;
    }
    else
    {
      z = center.getZ();
    }
    Anthrax::vec3<int> closest_point = Anthrax::vec3<int>(x, y, z);
    float distance = (closest_point - center).getMagnitude();
    load_quadrant[i] = distance <= load_distance;
  }

  // Find out if we need to read from a file
  if (layer_ == file_layer_)
  {
    VoxelSet voxel_set = VoxelSet(layer_);
    voxel_set.readFile(path_);
  }
}
