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


void VoxelDisplayList::push_back(Cube *new_cube)
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


void VoxelDisplayList::push_front(Cube *new_cube)
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
