/* ---------------------------------------------------------------- *\
 * voxeldisplaylist.cpp
 * Author: Gavin Ralston
 * Date Created: 2024-01-04
\* ---------------------------------------------------------------- */

#include "voxeldisplaylist.hpp"
#include <iostream>

namespace Anthrax
{


VoxelDisplayList::VoxelDisplayList()
{
  front_ = nullptr;
  back_ = nullptr;
  size_ = 0;
}


VoxelDisplayList::~VoxelDisplayList()
{
  while (front_ != nullptr)
  {
    VoxelDisplayListNode *tmp = front_;
    front_ = front_->next();
    delete tmp;
  }
}


VoxelDisplayList::iterator VoxelDisplayList::erase(iterator itr)
{
  if (size_ <= 0) return nullptr;

  iterator result = itr.node_->next_node_;
  size_--;
  // List containing a single node
  if (itr.node_ == front_ && front_ == back_)
  {
    front_ = back_ = nullptr;
  }
  // Removing the front
  else if (itr.node_ == front_)
  {
    front_ = front_->next_node_;
    front_->previous_node_ = nullptr;
  }
  // Removing the back
  else if (itr.node_ == back_)
  {
    back_ = back_->previous_node_;
    back_->next_node_ = nullptr;
  }
  // Normal removal
  else
  {
    itr.node_->previous_node_->next_node_ = itr.node_->next_node_;
    itr.node_->next_node_->previous_node_ = itr.node_->previous_node_;
  }
  delete(itr.node_);
  return result;
}


void VoxelDisplayList::push_back(std::weak_ptr<Cube> new_cube)
{
  if (front_ == nullptr)
  {
    front_ = new VoxelDisplayListNode(new_cube, nullptr, nullptr);
    back_ = front_;
  }
  else
  {
    back_ = new VoxelDisplayListNode(new_cube, back_, nullptr);
  }
  size_++;
}


void VoxelDisplayList::push_front(std::weak_ptr<Cube> new_cube)
{
  if (back_ == nullptr)
  {
    front_ = new VoxelDisplayListNode(new_cube);
    back_ = front_;
    return;
  }
  else
  {
    front_ = new VoxelDisplayListNode(new_cube, nullptr, front_);
  }
  size_++;
}


} // namespace Anthrax
