/* ---------------------------------------------------------------- *\
 * octree.cpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-12
\* ---------------------------------------------------------------- */
#include "octree.hpp"
#include <iostream>
#include <algorithm>
#include "cubeconvert.hpp"


Octree::Octree()
{
  Octree(0, 1);
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
    voxel_set_ = VoxelSet(layer_);
    voxel_set_.readFile(path_ + ".zn");
    is_uniform_ = voxel_set_.isUniform();
  }
  else if (layer_ == 0)
  {
    is_uniform_ = true;
  }
}


Octree::Octree(unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center, VoxelSet voxel_set)
{
  layer_ = layer;
  file_layer_ = file_layer;
  center_ = center;
  path_ = path;
  voxel_set_ = voxel_set;
  is_uniform_ = voxel_set_.isUniform();
}


Octree::~Octree()
{
  for (unsigned int i = 0; i < 8; i++)
  {
    if (children_[i] != NULL)
    {
      delete children_[i];
    }
  }
}

void Octree::setCubeSettingsFile(std::string file)
{
  cube_converter_.setFile(file);
}


void Octree::loadArea(Anthrax::vec3<int64_t> load_center, int load_distance)
{
  if (is_uniform_)
  {
    if (voxel_set_.getVoxelType() != 0)
    {
      for (unsigned int i = 0; i < 6; i++)
      {
        transparent_face_[i] = false;
      }
    }
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
      if (layer_ == 1)
        quadrant_centers[i].setX(center_.getX() - (quadrant_width >> 1) - 1);
      else
        quadrant_centers[i].setX(center_.getX() - (quadrant_width >> 1));
    }
    else
    {
      min_x = center_.getX();
      max_x = center_.getX() + quadrant_width-1;
      quadrant_centers[i].setX(center_.getX() + (quadrant_width >> 1));
    }

    if (i < 4)
    {
      min_y = center_.getY() - quadrant_width;
      max_y = center_.getY()-1;
      if (layer_ == 1)
        quadrant_centers[i].setY(center_.getY() - (quadrant_width >> 1) - 1);
      else
        quadrant_centers[i].setY(center_.getY() - (quadrant_width >> 1));
    }
    else
    {
      min_y = center_.getY();
      max_y = center_.getY() + quadrant_width-1;
      quadrant_centers[i].setY(center_.getY() + (quadrant_width >> 1));
    }
 
    if (i == 0 || i == 1 || i == 4 || i == 5)
    {
      min_z = center_.getZ() - quadrant_width;
      max_z = center_.getZ()-1;
      if (layer_ == 1)
        quadrant_centers[i].setZ(center_.getZ() - (quadrant_width >> 1) - 1);
      else
        quadrant_centers[i].setZ(center_.getZ() - (quadrant_width >> 1));
    }
    else
    {
      min_z = center_.getZ();
      max_z = center_.getZ() + quadrant_width-1;
      quadrant_centers[i].setZ(center_.getZ() + (quadrant_width >> 1));
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
    if (load_quadrant[i] && children_[i] != NULL)
    {
      children_[i]->loadArea(load_center, load_distance);
    }
  }

  // Check each face and set transparent_face_ values accordingly
  bool is_transparent = false;
  // Right face
  for (int i = 1; i < 8; i+=2)
  {
    if (children_[i] == NULL)
    {
      is_transparent = true;
      break;
    }
    if (children_[i]->faceIsTransparent(0))
    {
      is_transparent = true;
      break;
    }
  }
  transparent_face_[0] = is_transparent;
  // Left face
  is_transparent = false;
  for (int i = 0; i < 8; i+=2)
  {
    if (children_[i] == NULL)
    {
      is_transparent = true;
      break;
    }
    if (children_[i]->faceIsTransparent(1))
    {
      is_transparent = true;
      break;
    }
  }
  transparent_face_[1] = is_transparent;
  // Top face
  is_transparent = false;
  for (int i = 4; i < 8; i++)
  {
    if (children_[i] == NULL)
    {
      is_transparent = true;
      break;
    }
    if (children_[i]->faceIsTransparent(2))
    {
      is_transparent = true;
      break;
    }
  }
  transparent_face_[2] = is_transparent;
  // Bottom face
  is_transparent = false;
  for (int i = 0; i < 4; i++)
  {
    if (children_[i] == NULL)
    {
      is_transparent = true;
      break;
    }
    if (children_[i]->faceIsTransparent(3))
    {
      is_transparent = true;
      break;
    }
  }
  transparent_face_[3] = is_transparent;
  // Back face
  is_transparent = false;
  for (int i = 2; i < 8; i+=(i%2)*2+1)
  {
    if (children_[i] == NULL)
    {
      is_transparent = true;
      break;
    }
    if (children_[i]->faceIsTransparent(4))
    {
      is_transparent = true;
      break;
    }
  }
  transparent_face_[4] = is_transparent;
  // Front face
  is_transparent = false;
  for (int i = 0; i < 6; i+=(i%2)*2+1)
  {
    if (children_[i] == NULL)
    {
      is_transparent = true;
      break;
    }
    if (children_[i]->faceIsTransparent(5))
    {
      is_transparent = true;
      break;
    }
  }
  transparent_face_[5] = is_transparent;
}


void Octree::getNewNeighbors()
{
  if (is_uniform_) return;
  for (unsigned int i = 0; i < 8; i++)
  {
    if (children_[i] == NULL)
    {
      continue;
    }
    Octree *neighbor;
    int neighbor_child = 0;
    Octree *child_neighbors[6];
    if (i%2 == 0)
    {
      // Left side
      neighbor = neighbors_[0];
      if (neighbor == NULL) child_neighbors[0] = NULL; // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[0] = neighbor; // Uniform current neighbor
      else child_neighbors[0] = neighbor->getChildPointer(i+1);
      child_neighbors[1] = children_[i+1];
    }
    else
    {
      // Right side
      neighbor = neighbors_[1];
      if (neighbor == NULL) child_neighbors[1] = NULL; // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[1] = neighbor; // Uniform current neighbor
      else child_neighbors[1] = neighbor->getChildPointer(i-1);
      child_neighbors[0] = children_[i-1];
    }
    if (i < 4)
    {
      // Bottom side
      neighbor = neighbors_[2];
      if (neighbor == NULL) child_neighbors[2] = NULL; // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[2] = neighbor; // Uniform current neighbor
      else child_neighbors[2] = neighbor->getChildPointer(i+4);
      child_neighbors[3] = children_[i+4];
    }
    else
    {
      // Top side
      neighbor = neighbors_[3];
      if (neighbor == NULL) child_neighbors[3] = NULL; // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[3] = neighbor; // Uniform current neighbor
      else child_neighbors[3] = neighbor->getChildPointer(i-4);
      child_neighbors[2] = children_[i-4];
    }
    if (i%4 < 2)
    {
      // Front side
      neighbor = neighbors_[4];
      if (neighbor == NULL) child_neighbors[4] = NULL; // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[4] = neighbor; // Uniform current neighbor
      else child_neighbors[4] = neighbor->getChildPointer(i+2);
      child_neighbors[5] = children_[i+2];
    }
    else
    {
      // Back side
      neighbor = neighbors_[5];
      if (neighbor == NULL) child_neighbors[5] = NULL; // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[5] = neighbor; // Uniform current neighbor
      else child_neighbors[5] = neighbor->getChildPointer(i-2);
      child_neighbors[4] = children_[i-2];
    }
    children_[i]->setNeighbors(child_neighbors);
    children_[i]->getNewNeighbors();
  }
}


void Octree::setNeighbors(Octree **neighbors)
{
  //std::copy(neighbors, neighbors+6, neighbors_);
  for (unsigned int i = 0; i < 6; i++)
  {
    neighbors_[i] = neighbors[i];
  }
}


void Octree::getCubes(std::vector<Anthrax::Cube> *cube_vector)
{
  if (is_uniform_)
  {
    if (voxel_set_.getVoxelType() == 0) return;
    Anthrax::vec3<float> center;
    center.setX(floor(center_.getX()));
    center.setY(floor(center_.getY()));
    center.setZ(floor(center_.getZ()));
    if (!(layer_ == 0)) center = center - Anthrax::vec3<float>(0.5, 0.5, 0.5);

    //cube_vector->push_back(cube_converter_.convert(voxel_set_.getVoxelType(), center, 1 << layer_));
     
    //cube_vector->push_back(Anthrax::Cube(Anthrax::vec4<float>(0.5, 0.1, 0.8, 1.0), center, 1 << (layer_)));
    bool render_face[6];
    bool render_cube = false;
    for (unsigned int i = 0; i < 6; i++)
    {
      if (neighbors_[i] == NULL)
      {
        render_face[i] = true;
        render_cube = true;
        continue;
      }
      //render_face[i] = neighbors_[i]->faceIsTransparent(i%2 == 0 ? i+1 : i-1);
      render_face[i] = neighbors_[i]->faceIsTransparent(i);
      if (render_face[i]) render_cube = true;
    }
    if (!render_cube) return; // No faces are visible, so don't draw this cube
    Anthrax::Cube cube = cube_converter_.convert(voxel_set_.getVoxelType(), center, 1 << layer_);
    cube.setFaces(render_face);
    cube_vector->push_back(cube);
    //(*cube_map)[cube.getTypeID()].push_back(cube);
    return;
  }
  for (unsigned int i = 0; i < 8; i++)
  {
    if (children_[i] != NULL)
    {
      children_[i]->getCubes(cube_vector);
    }
  }
}
