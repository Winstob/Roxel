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
    front_ = new VoxelDisplayListNode(new_cube);
    back_ = front_;
    return;
  }

  back_ = new VoxelDisplayListNode(new_cube, back_, nullptr);
}


void VoxelDisplayList::push_front(Cube *new_cube)
{
  if (back_ == nullptr)
  {
    front_ = new VoxelDisplayListNode(new_cube);
    back_ = front_;
    return;
  }

  front_ = new VoxelDisplayListNode(new_cube, back_, nullptr);
}


} // namespace Anthrax
