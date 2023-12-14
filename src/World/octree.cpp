/* ---------------------------------------------------------------- *\
 * octree.cpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-12
\* ---------------------------------------------------------------- */
#include "octree.hpp"
#include <iostream>


Octree::Octree()
{
  Octree(0, 0);
}


Octree::Octree(unsigned int layer, unsigned int file_layer)
{
  Octree(layer, file_layer, "", Anthrax::vec3<int64_t>(0, 0, 0));
}


Octree::Octree(unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center)
{
  layer_ = layer;
  file_layer_ = file_layer;
  center_ = center;
  is_uniform_ = false;
  path_ = path;

  if (layer_ == file_layer_)
  {
    /*
    voxel_set_ = VoxelSet(layer_);
    voxel_set_.readFile(path_ + ".zn");
    is_uniform_ = voxel_set_.isUniform();
    */
  }
 
  //if (layer_ > file_layer_)
  std::cout << path_ << std::endl;
  std::cout << "<" << center_.getX() << ", " << center_.getY() << ", " << center_.getZ() << ">" << std::endl;
}


Octree::Octree(unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center, VoxelSet voxel_set)
{
  Octree(layer, file_layer, path, center);
  voxel_set_ = voxel_set;
  is_uniform_ = voxel_set_.isUniform();
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


void Octree::loadArea(Anthrax::vec3<int64_t> load_center, int load_distance)
{
  //std::cout << center_.getX() << std::endl;
  if (is_uniform_)
  {
    return;
  }
  int64_t closest_x, closest_y, closest_z;

  // Might as well find the new centers now since we'll need them later
  Anthrax::vec3<int64_t> quadrant_centers[8];

  // Iterate through all 8 quadrants to find which ones need to be loaded
  bool load_quadrant[8];
  int64_t quadrant_width = (1LL << (layer_-1)); // 2^(layer_-1)
  //std::cout << (quadrant_width) << std::endl;
  for (unsigned int i = 0; i < 8; i++)
  {
    // Find the closest point to the center of the desired loading area
    int64_t min_x, max_x, min_y, max_y, min_z, max_z;
    if (i%2 == 0)
    {
      min_x = center_.getX() - quadrant_width;
      max_x = center_.getX()-1;
      /*
      std::cout << center_.getX() << std::endl;
      std::cout << (quadrant_width/2) << std::endl;
      std::cout << (center_.getX() - ceil((float)quadrant_width/2)) << std::endl;
      */
      quadrant_centers[i].setX(center_.getX() - ceil((float)quadrant_width/2));
    }
    else
    {
      min_x = center_.getX();
      max_x = center_.getX() + quadrant_width-1;
      quadrant_centers[i].setX(center_.getX() + floor((float)quadrant_width/2));
    }

    if (i < 4)
    {
      min_y = center_.getY() - quadrant_width;
      max_y = center_.getY()-1;
      quadrant_centers[i].setY(center_.getY() - ceil((float)quadrant_width/2));
    }
    else
    {
      min_y = center_.getY();
      max_y = center_.getY() + quadrant_width-1;
      quadrant_centers[i].setY(center_.getY() + floor((float)quadrant_width/2));
    }
 
    if (i == 0 || i == 1 || i == 4 || i == 5)
    {
      min_z = center_.getZ() - quadrant_width;
      max_z = center_.getZ()-1;
      quadrant_centers[i].setZ(center_.getZ() - ceil((float)quadrant_width/2));
    }
    else
    {
      min_z = center_.getZ();
      max_z = center_.getZ() + quadrant_width-1;
      quadrant_centers[i].setZ(center_.getZ() + floor((float)quadrant_width/2));
    }

    // Now solve for the closest point
    if (load_center.getX() <= min_x)
    {
      closest_x = min_x;
    }
    else if (load_center.getX() >= max_x)
    {
      closest_x = max_x;
    }
    else
    {
      closest_x = load_center.getX();
    }

    if (load_center.getY() <= min_y)
    {
      closest_y = min_y;
    }
    else if (load_center.getY() >= max_y)
    {
      closest_y = max_y;
    }
    else
    {
      closest_y = load_center.getY();
    }

    if (load_center.getZ() <= min_z)
    {
      closest_z = min_z;
    }
    else if (load_center.getZ() >= max_z)
    {
      closest_z = max_z;
    }
    else
    {
      closest_z = load_center.getZ();
    }
    Anthrax::vec3<int64_t> closest_point = Anthrax::vec3<int64_t>(closest_x, closest_y, closest_z);
    float distance = (closest_point - load_center).getMagnitude();
    load_quadrant[i] = distance <= load_distance;
    //std::cout << load_quadrant[i] << std::endl;
  }

  if (layer_ <= file_layer_)
  {
    for (unsigned int i = 0; i < 8; i++)
    {
      if (load_quadrant[i])
      {
        children_[i] = new Octree(layer_ - 1, file_layer_, path_ + std::to_string(i), quadrant_centers[i], voxel_set_.getQuadrant(i));
      }
    }
  }
  else
  {
    for (unsigned int i = 0; i < 8; i++)
    {
      if (load_quadrant[i])
      {
        children_[i] = new Octree(layer_ - 1, file_layer_, path_ + std::to_string(i), quadrant_centers[i]);
      }
    }
  }

  for (unsigned int i = 0; i < 8; i++)
  {
    if (children_[i] != NULL)
    {
      children_[i]->loadArea(load_center, load_distance);
    }
  }
}
