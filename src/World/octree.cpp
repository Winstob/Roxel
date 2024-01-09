/* ---------------------------------------------------------------- *\
 * octree.cpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-12
\* ---------------------------------------------------------------- */
#include "octree.hpp"
#include <iostream>
#include <algorithm>
#include "cubeconvert.hpp"


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
  is_leaf_ = is_uniform_;
  cube_pointer_ = nullptr;
}


Octree::Octree(unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center, VoxelSet voxel_set)
{
  layer_ = layer;
  file_layer_ = file_layer;
  center_ = center;
  path_ = path;
  voxel_set_ = voxel_set;
  is_uniform_ = voxel_set_.isUniform();
  is_leaf_ = is_uniform_;
  cube_pointer_ = nullptr;
}


Octree::~Octree()
{
  for (unsigned int i = 0; i < 8; i++)
  {
    if (children_[i] != nullptr)
    {
      delete children_[i];
      children_[i] = nullptr;
    }
  }
  if (cube_pointer_ != nullptr)
  {
    cube_pointer_.reset();
    cube_pointer_ = nullptr;
  }
}

void Octree::setCubeSettingsFile(std::string file)
{
  cube_converter_.setFile(file);
}


void Octree::setAnthraxPointer(Anthrax::Anthrax *anthrax_instance)
{
  anthrax_instance_ = anthrax_instance;
}


void Octree::setLoadDecisionFunction(bool (*load_decision_function)(uint64_t, int))
{
  loadDecisionFunction = load_decision_function;
}


void Octree::loadArea(Anthrax::vec3<int64_t> load_center, int load_distance)
{
  bool was_leaf = is_leaf_;
  is_leaf_ = true;

  int64_t closest_x, closest_y, closest_z;

  // Might as well find the new centers now since we'll need them later
  Anthrax::vec3<int64_t> quadrant_centers[8];

  // Iterate through all 8 quadrants to find which ones need to be loaded
  bool load_quadrant[8] = {false};
  int64_t quadrant_width = (1LL << (layer_-1)); // 2^(layer_-1)
  //std::cout << (quadrant_width) << std::endl;
  for (unsigned int i = 0; i < 8; i++)
  {
    /*
    if (children_[i])
    {
      load_quadrant[i] = true;
      is_leaf_ = false;
      continue;
    }
    */
    // Approximate the distance of the closest point to the center of the desired loading area
    if (i%2 == 0)
    {
      if (layer_ == 1)
        quadrant_centers[i].setX(center_.getX() - (quadrant_width >> 1) - 1);
      else
        quadrant_centers[i].setX(center_.getX() - (quadrant_width >> 1));
    }
    else
    {
      quadrant_centers[i].setX(center_.getX() + (quadrant_width >> 1));
    }

    if (i < 4)
    {
      if (layer_ == 1)
        quadrant_centers[i].setY(center_.getY() - (quadrant_width >> 1) - 1);
      else
        quadrant_centers[i].setY(center_.getY() - (quadrant_width >> 1));
    }
    else
    {
      quadrant_centers[i].setY(center_.getY() + (quadrant_width >> 1));
    }
 
    if (i == 0 || i == 1 || i == 4 || i == 5)
    {
      if (layer_ == 1)
        quadrant_centers[i].setZ(center_.getZ() - (quadrant_width >> 1) - 1);
      else
        quadrant_centers[i].setZ(center_.getZ() - (quadrant_width >> 1));
    }
    else
    {
      quadrant_centers[i].setZ(center_.getZ() + (quadrant_width >> 1));
    }
    float distance = (quadrant_centers[i] - load_center).getMagnitude() - (quadrant_width * 0.866025403784);
    if (distance < 0) distance = 0;
    load_quadrant[i] = loadDecisionFunction(distance, layer_-1);
    if (is_leaf_ && load_quadrant[i])
    {
      is_leaf_ = false;
    }
  }

  if (!was_leaf && is_leaf_)
  {
    // The current layer was not a leaf before (has children) but now it is
    // delete children
    for (unsigned int i = 0; i < 8; i++)
    {
      if (children_[i] != nullptr)
      {
        delete(children_[i]);
        children_[i] = nullptr;
      }
    }

    if (voxel_set_.getVoxelType() != 0)
    {
      for (unsigned int i = 0; i < 6; i++)
      {
        transparent_face_[i] = false;
      }
    }
    return;
  }
  else if (was_leaf && !is_leaf_)
  {
    if (cube_pointer_ != nullptr)
    {
      cube_pointer_.reset();
      cube_pointer_ = nullptr;
    }
  }


  if (layer_ <= file_layer_)
  {
    for (unsigned int i = 0; i < 8; i++)
    {
      if (load_quadrant[i] && children_[i] == nullptr)
      {
        children_[i] = new Octree(layer_ - 1, file_layer_, path_ + std::to_string(i), quadrant_centers[i], voxel_set_.getQuadrant(i));
      }
    }
  }
  else
  {
    for (unsigned int i = 0; i < 8; i++)
    {
      if (load_quadrant[i] && children_[i] == nullptr)
      {
        children_[i] = new Octree(layer_ - 1, file_layer_, path_ + std::to_string(i), quadrant_centers[i]);
      }
    }
  }

  for (unsigned int i = 0; i < 8; i++)
  {
    if (load_quadrant[i])
    {
      children_[i]->loadArea(load_center, load_distance);
    }
  }

  // Check each face and set transparent_face_ values accordingly
  bool is_transparent = false;
  // Right face
  for (int i = 1; i < 8; i+=2)
  {
    if (!children_[i] || children_[i]->faceIsTransparent(0))
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
    if (!children_[i] || children_[i]->faceIsTransparent(1))
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
    if (!children_[i] || children_[i]->faceIsTransparent(2))
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
    if (!children_[i] || children_[i]->faceIsTransparent(3))
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
    if (!children_[i] || children_[i]->faceIsTransparent(4))
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
    if (!children_[i] || children_[i]->faceIsTransparent(5))
    {
      is_transparent = true;
      break;
    }
  }
  transparent_face_[5] = is_transparent;
}


void Octree::getNewNeighbors()
{
  if (is_leaf_ || is_uniform_) return;
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


void Octree::getCubes()
{
  /*
  if (cube_pointer_)
  {
    delete(cube_pointer_);
  }
  */
  if (is_leaf_)
  {
    if (!cube_pointer_)
    {
      if (voxel_set_.getVoxelType() == 0) return;
      Anthrax::vec3<float> center;
      center.setX(floor(center_.getX()));
      center.setY(floor(center_.getY()));
      center.setZ(floor(center_.getZ()));
      if (!(layer_ == 0)) center = center - Anthrax::vec3<float>(0.5, 0.5, 0.5);

      //cube_vector->push_back(cube_converter_.convert(voxel_set_.getVoxelType(), center, 1 << layer_));
       
      //cube_vector->push_back(Anthrax::Cube(Anthrax::vec4<float>(0.5, 0.1, 0.8, 1.0), center, 1 << (layer_)));
      bool render_face[6] = {false};
      bool render_cube = false;
      for (unsigned int i = 0; i < 6; i++)
      {
        if (!neighbors_[i] || neighbors_[i]->faceIsTransparent(i))
        {
          render_face[i] = true;
          render_cube = true;
        }
        //render_face[i] = neighbors_[i]->faceIsTransparent(i%2 == 0 ? i+1 : i-1);
      }
      if (!render_cube) return; // No faces are visible, so don't draw this cube
      cube_pointer_.reset(new Anthrax::Cube());
      *cube_pointer_ = cube_converter_.convert(voxel_set_.getVoxelType(), center, 1 << layer_);
      cube_pointer_->setFaces(render_face);
      anthrax_instance_->addVoxel(cube_pointer_);
      //cube_vector->push_back(cube);
    }
  }
  else
  {
    /*
    if (cube_pointer_)
    {
      delete(cube_pointer_);
      std::cout << "POAI" << std::endl;
    }
    */
    for (unsigned int i = 0; i < 8; i++)
    {
      if (children_[i])
      {
        children_[i]->getCubes();
      }
    }
  }
}
