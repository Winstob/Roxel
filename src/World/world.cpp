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
  octree_ = std::make_shared<Octree>(Octree(std::make_shared<Octree>(), num_layers_, zone_depth_, directory_ + "/", Anthrax::vec3<int64_t>(0, 0, 0)));
  octree_->setCubeSettingsFile("voxelmap.json");

  anthrax_instance_ = anthrax_instance;
  octree_->setAnthraxPointer(anthrax_instance_);
  octree_->setLoadDecisionFunction([](uint64_t distance, int layer) {
      return (distance < 5000 && layer > distance / 500);
      });
  leaves_.push_back(octree_);
  current_leaf_itr_ = leaves_.begin();
}


void World::loadAreaRecursive(Anthrax::vec3<int64_t> center)
{
  octree_->loadAreaRecursive(center);
  octree_->getNewNeighbors();
  octree_->getCubes();
  return;
}


void World::loadArea(Anthrax::vec3<int64_t> center)
{
  //Anthrax::List<Octree>::iterator current_leaf_itr = leaves_.begin();


  int count = 0;
  //current_leaf_itr_ = nullptr;
  Anthrax::List<Octree>::iterator prev_itr = nullptr;
  while (count < 1000 && current_leaf_itr_ != nullptr)
  {
    if (prev_itr == current_leaf_itr_)
    {
      current_leaf_itr_++;
    }
    prev_itr = current_leaf_itr_;
    if (auto current_leaf = (*current_leaf_itr_).lock())
    {
      auto parent = current_leaf->getParentPointer().lock();
      float distance = (current_leaf->getCenter() - center).getMagnitude() - ((1LL << current_leaf->getLayer()) * 0.866025403784);
      if (distance < 0) distance = 0;
      //std::cout << current_leaf->getCenter().getX() << " " << current_leaf->getCenter().getY() << std::endl;
      if (current_leaf->loadDecisionFunction(distance, current_leaf->getLayer()))
      {
        // Load this leaf's children
        current_leaf->loadChildren();
        if (!(current_leaf->isLeaf()))
        {
          for (unsigned int i = 0; i < 8; i++)
          {
            leaves_.push_back(current_leaf->getChildPointer(i));
          }
          current_leaf_itr_ = leaves_.erase(current_leaf_itr_);
        }
        // This leaf is loaded, so we don't need to check the parent in the future
        if (!(current_leaf->parent_load_checked) && parent != nullptr)
        {
          for (unsigned int i = 0; i < 8; i++)
          {
            parent->getChildPointer(i).lock()->parent_load_checked = true;
          }
        }
        // Now we are done with this leaf
        current_leaf->parent_load_checked = false;
        count++;
      }
      else
      {
        // This leaf's children should not be loaded - check the parent to see if this leaf should remain loaded
        if (!(current_leaf->parent_load_checked))
        {
          for (unsigned int i = 0; i < 8; i++)
          {
            parent->getChildPointer(i).lock()->parent_load_checked = true;
          }
          distance = (parent->getCenter() - center).getMagnitude() - ((1LL << parent->getLayer()) * 0.866025403784);
          if (distance < 0) distance = 0;
          if (!(parent->loadDecisionFunction(distance, parent->getLayer())))
          {
            parent->deleteChildren();
            leaves_.push_back(parent);
            current_leaf_itr_ = leaves_.erase(current_leaf_itr_);
            count++;
          }
        }
        else
        {
          current_leaf->parent_load_checked = false;
          current_leaf_itr_++;
          count++;
        }
      }
    }
    else
    {
      current_leaf_itr_ = leaves_.erase(current_leaf_itr_);
      continue;
    }
  }
  if (current_leaf_itr_ == nullptr)
  {
    current_leaf_itr_ = leaves_.begin();
    std::cout << "POOP" << std::endl;
  }

  octree_->getNewNeighbors();
  octree_->getCubes();
}


void World::getCubes()
{
  octree_->getCubes();
}
