/* ---------------------------------------------------------------- *\
 * voxeldisplaylistnode.cpp
 * Author: Gavin Ralston
 * Date Created: 2024-01-04
\* ---------------------------------------------------------------- */

#include "voxeldisplaylistnode.hpp"

namespace Anthrax
{


VoxelDisplayListNode::VoxelDisplayListNode(Cube *cube, VoxelDisplayListNode *previous_node, VoxelDisplayListNode *next_node)
{
  cube_ = cube;

  if (previous_node != nullptr)
  {
    previous_node->setNext(this);
  }
  previous_node_ = previous_node;

  if (next_node != nullptr)
  {
    next_node->setPrevious(this);
  }
  next_node_ = next_node;
}



VoxelDisplayListNode::~VoxelDisplayListNode()
{
  // Deletion of the cube pointers is up to the user, so we don't explicitly delete them here
}


void VoxelDisplayListNode::setPrevious(VoxelDisplayListNode *new_previous_node)
{
  previous_node_ = new_previous_node;
}


void VoxelDisplayListNode::setNext(VoxelDisplayListNode *new_next_node)
{
  next_node_ = new_next_node;
}


} // namespace Anthrax
