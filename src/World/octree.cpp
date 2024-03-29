/* ---------------------------------------------------------------- *\
 * octree.cpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-12
\* ---------------------------------------------------------------- */
#include "octree.hpp"
#include <iostream>
#include <algorithm>
#include "cubeconvert.hpp"


Octree::Octree(std::weak_ptr<Octree> parent, unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center)
{
  parent_ = parent;
  layer_ = layer;
  file_layer_ = file_layer;
  center_ = center;
  is_uniform_ = false;
  path_ = path;

  if (layer_ == file_layer_)
  {
    voxel_set_ = VoxelSet(1 << (3*layer_));
    voxel_set_.readFile(path_ + ".zn");
    is_uniform_ = voxel_set_.isUniform();
    splitVoxelSet();
  }
  else if (layer_ == 0)
  {
    is_uniform_ = true;
  }
  is_leaf_ = true;
  cube_pointer_ = nullptr;
}


Octree::Octree(std::weak_ptr<Octree> parent, unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center, VoxelSet voxel_set)
{
  parent_ = parent;
  layer_ = layer;
  file_layer_ = file_layer;
  center_ = center;
  path_ = path;
  voxel_set_ = voxel_set;
  is_uniform_ = voxel_set_.isUniform();
  if (layer_ != 0)
    splitVoxelSet();
  is_leaf_ = true;
  cube_pointer_ = nullptr;
}


Octree::~Octree()
{
  for (unsigned int i = 0; i < 8; i++)
  {
    if (children_[i] != nullptr)
    {
      children_[i].reset();
      children_[i] = nullptr;
    }
  }
  if (cube_pointer_ != nullptr)
  {
    cube_pointer_.reset();
    cube_pointer_ = nullptr;
  }
}


void Octree::splitVoxelSet()
{
  /*
  VoxelSet halves[2];
  voxel_set_.bisect(&(halves[0]), &(halves[1]));
  VoxelSet quarters[4];
  halves[0].bisect(&(quarters[0]), &(quarters[1]));
  halves[1].bisect(&(quarters[2]), &(quarters[3]));

  quarters[0].bisect(&(voxel_set_quadrants_[0]), &(voxel_set_quadrants_[1]));
  quarters[1].bisect(&(voxel_set_quadrants_[2]), &(voxel_set_quadrants_[3]));
  quarters[2].bisect(&(voxel_set_quadrants_[4]), &(voxel_set_quadrants_[5]));
  quarters[3].bisect(&(voxel_set_quadrants_[6]), &(voxel_set_quadrants_[7]));
  */
  // Split in half
  voxel_set_.bisect(&(voxel_set_quadrants_[2]), &(voxel_set_quadrants_[3]));
  // Split in quarters
  voxel_set_quadrants_[2].bisect(&(voxel_set_quadrants_[4]), &(voxel_set_quadrants_[5]));
  voxel_set_quadrants_[3].bisect(&(voxel_set_quadrants_[6]), &(voxel_set_quadrants_[7]));
  // Split in eighths
  voxel_set_quadrants_[4].bisect(&(voxel_set_quadrants_[0]), &(voxel_set_quadrants_[1]));
  voxel_set_quadrants_[5].bisect(&(voxel_set_quadrants_[2]), &(voxel_set_quadrants_[3]));
  voxel_set_quadrants_[6].bisect(&(voxel_set_quadrants_[4]), &(voxel_set_quadrants_[5]));
  voxel_set_quadrants_[7].bisect(&(voxel_set_quadrants_[6]), &(voxel_set_quadrants_[7]));
  return;
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


void Octree::loadAreaRecursive(Anthrax::vec3<int64_t> load_center)
{
  if (is_uniform_) 
  {
    is_leaf_ = true;
    if (voxel_set_.getVoxelType() != 0)
    {
      for (unsigned int i = 0; i < 6; i++)
      {
        transparent_face_[i] = false;
      }
    }
    return;
  }


  bool was_leaf = is_leaf_;
  is_leaf_ = true;
  Anthrax::vec3<int64_t> quadrant_centers[8];

  float distance = (center_ - load_center).getMagnitude() - ((1LL << layer_) * 0.866025403784);
  if (distance < 0) distance = 0;
  if (loadDecisionFunction(distance, layer_))
  {
    is_leaf_ = false;

    // Iterate through all 8 quadrants to find the centers of each
    int64_t quadrant_width = (1LL << (layer_-1)); // 2^(layer_-1)
    for (unsigned int i = 0; i < 8; i++)
    {
      if (children_[i] != nullptr)
      {
        // Don't need to calculate the center since it won't be used anyway
        continue;
      }
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
    }
  }

  if (is_leaf_)
  {
    for (unsigned int i = 0; i < 8; i++)
    {
      if (children_[i] != nullptr)
      {
        // The current layer was not a leaf before (has children) but now it is
        // delete children
        children_[i].reset();
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

  if (!is_leaf_)
  {
    if (layer_ <= file_layer_)
    {
      for (unsigned int i = 0; i < 8; i++)
      {
        if (children_[i] == nullptr)
        {
          //children_[i] = std::make_shared<Octree>(Octree(this, layer_ - 1, file_layer_, path_ + std::to_string(i), quadrant_centers[i], voxel_set_.getQuadrant(i)));
          children_[i] = std::make_shared<Octree>(Octree(weak_from_this(), layer_ - 1, file_layer_, path_ + std::to_string(i), quadrant_centers[i], voxel_set_quadrants_[i]));
        }
      }
    }
    else
    {
      for (unsigned int i = 0; i < 8; i++)
      {
        if (children_[i] == nullptr)
        {
          children_[i] = std::make_shared<Octree>(Octree(weak_from_this(), layer_ - 1, file_layer_, path_ + std::to_string(i), quadrant_centers[i]));
        }
      }
    }

    for (unsigned int i = 0; i < 8; i++)
    {
      if (children_[i] != nullptr)
        children_[i]->loadAreaRecursive(load_center);
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
  if (transparent_face_[0] != is_transparent)
  {
    if (auto tmp = neighbors_[1].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[1] != nullptr) neighbors_[1]->neighbors_changed_ = true;
    transparent_face_[0] = is_transparent;
  }
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
  if (transparent_face_[1] != is_transparent)
  {
    if (auto tmp = neighbors_[0].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[0] != nullptr) neighbors_[0]->neighbors_changed_ = true;
    transparent_face_[1] = is_transparent;
  }
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
  if (transparent_face_[2] != is_transparent)
  {
    if (auto tmp = neighbors_[3].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[3] != nullptr) neighbors_[3]->neighbors_changed_ = true;
    transparent_face_[2] = is_transparent;
  }
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
  if (transparent_face_[3] != is_transparent)
  {
    if (auto tmp = neighbors_[2].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[2] != nullptr) neighbors_[2]->neighbors_changed_ = true;
    transparent_face_[3] = is_transparent;
  }
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
  if (transparent_face_[4] != is_transparent)
  {
    if (auto tmp = neighbors_[5].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[5] != nullptr) neighbors_[5]->neighbors_changed_ = true;
    transparent_face_[4] = is_transparent;
  }
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
  if (transparent_face_[5] != is_transparent)
  {
    if (auto tmp = neighbors_[4].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[4] != nullptr) neighbors_[4]->neighbors_changed_ = true;
    transparent_face_[5] = is_transparent;
  }
}


void Octree::getNewNeighbors()
{
  //if (is_leaf_ || is_uniform_) return;
  for (unsigned int i = 0; i < 8; i++)
  {
    if (children_[i] == nullptr)
    {
      continue;
    }
    std::shared_ptr<Octree> neighbor;
    int neighbor_child = 0;
    std::weak_ptr<Octree> child_neighbors[6];

    if (i%2 == 0)
    {
      // Left side
      neighbor = neighbors_[0].lock();
      if (neighbor == nullptr) child_neighbors[0] = std::weak_ptr<Octree>(); // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[0] = neighbors_[0]; // Uniform current neighbor
      else child_neighbors[0] = neighbor->getChildPointer(i+1);
      child_neighbors[1] = children_[i+1];
    }
    else
    {
      // Right side
      neighbor = neighbors_[1].lock();
      if (neighbor == nullptr) child_neighbors[1] = std::weak_ptr<Octree>(); // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[1] = neighbor; // Uniform current neighbor
      else child_neighbors[1] = neighbor->getChildPointer(i-1);
      child_neighbors[0] = children_[i-1];
    }
    if (i < 4)
    {
      // Bottom side
      neighbor = neighbors_[2].lock();
      if (neighbor == nullptr) child_neighbors[2] = std::weak_ptr<Octree>(); // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[2] = neighbor; // Uniform current neighbor
      else child_neighbors[2] = neighbor->getChildPointer(i+4);
      child_neighbors[3] = children_[i+4];
    }
    else
    {
      // Top side
      neighbor = neighbors_[3].lock();
      if (neighbor == nullptr) child_neighbors[3] = std::weak_ptr<Octree>(); // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[3] = neighbor; // Uniform current neighbor
      else child_neighbors[3] = neighbor->getChildPointer(i-4);
      child_neighbors[2] = children_[i-4];
    }
    if (i%4 < 2)
    {
      // Front side
      neighbor = neighbors_[4].lock();
      if (neighbor == nullptr) child_neighbors[4] = std::weak_ptr<Octree>(); // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[4] = neighbor; // Uniform current neighbor
      else child_neighbors[4] = neighbor->getChildPointer(i+2);
      child_neighbors[5] = children_[i+2];
    }
    else
    {
      // Back side
      neighbor = neighbors_[5].lock();
      if (neighbor == nullptr) child_neighbors[5] = std::weak_ptr<Octree>(); // Edge of map/loaded area
      else if (neighbor->isUniform()) child_neighbors[5] = neighbor; // Uniform current neighbor
      else child_neighbors[5] = neighbor->getChildPointer(i-2);
      child_neighbors[4] = children_[i-2];
    }
    children_[i]->setNeighbors(child_neighbors);
    children_[i]->getNewNeighbors();
  }
}


void Octree::loadChildren()
{
  if (is_uniform_) 
  {
    is_leaf_ = true;
    if (voxel_set_.getVoxelType() != 0)
    {
      for (unsigned int i = 0; i < 6; i++)
      {
        transparent_face_[i] = false;
      }
    }
    return;
  }


  bool was_leaf = is_leaf_;
  is_leaf_ = false;
  Anthrax::vec3<int64_t> quadrant_centers[8];

  // Iterate through all 8 quadrants to find the centers of each
  int64_t quadrant_width = (1LL << (layer_-1)); // 2^(layer_-1)
  for (unsigned int i = 0; i < 8; i++)
  {
    if (children_[i] != nullptr)
    {
      // Don't need to calculate the center since it won't be used anyway
      continue;
    }
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
  }

  if (was_leaf)
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
      if (children_[i] == nullptr)
      {
        //children_[i] = std::make_shared<Octree>(Octree(weak_from_this(), layer_ - 1, file_layer_, path_ + std::to_string(i), quadrant_centers[i], voxel_set_.getQuadrant(i)));
        children_[i] = std::make_shared<Octree>(Octree(weak_from_this(), layer_ - 1, file_layer_, path_ + std::to_string(i), quadrant_centers[i], voxel_set_quadrants_[i]));
      }
    }
  }
  else
  {
    for (unsigned int i = 0; i < 8; i++)
    {
      if (children_[i] == nullptr)
      {
        children_[i] = std::make_shared<Octree>(Octree(weak_from_this(), layer_ - 1, file_layer_, path_ + std::to_string(i), quadrant_centers[i]));
      }
    }
  }
  //if (was_leaf == is_leaf_) return;

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
  if (transparent_face_[0] != is_transparent)
  {
    if (auto tmp = neighbors_[1].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[1] != nullptr) neighbors_[1]->neighbors_changed_ = true;
    transparent_face_[0] = is_transparent;
  }
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
  if (transparent_face_[1] != is_transparent)
  {
    if (auto tmp = neighbors_[0].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[0] != nullptr) neighbors_[0]->neighbors_changed_ = true;
    transparent_face_[1] = is_transparent;
  }
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
  if (transparent_face_[2] != is_transparent)
  {
    if (auto tmp = neighbors_[3].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[3] != nullptr) neighbors_[3]->neighbors_changed_ = true;
    transparent_face_[2] = is_transparent;
  }
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
  if (transparent_face_[3] != is_transparent)
  {
    if (auto tmp = neighbors_[2].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[2] != nullptr) neighbors_[2]->neighbors_changed_ = true;
    transparent_face_[3] = is_transparent;
  }
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
  if (transparent_face_[4] != is_transparent)
  {
    if (auto tmp = neighbors_[5].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[5] != nullptr) neighbors_[5]->neighbors_changed_ = true;
    transparent_face_[4] = is_transparent;
  }
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
  if (transparent_face_[5] != is_transparent)
  {
    if (auto tmp = neighbors_[4].lock()) tmp->neighbors_changed_ = true;
    //if (neighbors_[4] != nullptr) neighbors_[4]->neighbors_changed_ = true;
    transparent_face_[5] = is_transparent;
  }
}


void Octree::deleteChildren()
{
  is_leaf_ = true;
  for (unsigned int i = 0; i < 8; i++)
  {
    if (children_[i] != nullptr)
    {
      children_[i].reset();
      children_[i] = nullptr;
    }
  }
  return;
}



void Octree::setNeighbors(std::weak_ptr<Octree> *neighbors)
{
  //std::copy(neighbors, neighbors+6, neighbors_);
  for (unsigned int i = 0; i < 6; i++)
  {
    //if (neighbors_[i] != neighbors[i]) neighbors_changed_[i] = true;
    neighbors_[i] = neighbors[i];
  }
}


void Octree::getCubes()
{
  if (is_leaf_)
  {
    if (cube_pointer_ != nullptr)
    {
      // Take into account that some existing cube pointers may have updated neighbors, so redraw them
      if (neighbors_changed_)
      {
        cube_pointer_.reset();
        cube_pointer_ = nullptr;
        neighbors_changed_ = false;
      }
    }

    bool render_face[6] = {false};
    bool render_cube = false;
    for (unsigned int i = 0; i < 6; i++)
    {
      if (neighbors_[i].expired() || neighbors_[i].lock()->faceIsTransparent(i))
      {
        render_face[i] = true;
        render_cube = true;
      }
      //render_face[i] = neighbors_[i]->faceIsTransparent(i%2 == 0 ? i+1 : i-1);
    }
    if (!render_cube) // No faces are visible, so don't draw this cube
    {
      cube_pointer_.reset();
      cube_pointer_ = nullptr;
      return;
    }

    if (!cube_pointer_)
    {
      if (voxel_set_.getVoxelType() == 0) return;
      Anthrax::vec3<float> center;
      center.setX(floor(center_.getX()));
      center.setY(floor(center_.getY()));
      center.setZ(floor(center_.getZ()));
      if (!(layer_ == 0)) center = center - Anthrax::vec3<float>(0.5, 0.5, 0.5);

      cube_pointer_.reset(new Anthrax::Cube());
      *cube_pointer_ = cube_converter_.convert(voxel_set_.getVoxelType(), center, 1 << layer_);
      cube_pointer_->setFaces(render_face);
      anthrax_instance_->addVoxel(cube_pointer_);
    }
  }
  else
  {
    for (unsigned int i = 0; i < 8; i++)
    {
      if (children_[i] != nullptr)
      {
        children_[i]->getCubes();
      }
    }
  }
}
