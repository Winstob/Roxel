/* ---------------------------------------------------------------- *\
 * voxeldisplaylistnode.hpp
 * Author: Gavin Ralston
 * Date Created: 2024-01-04
\* ---------------------------------------------------------------- */

#ifndef VOXELDISPLAYLISTNODE_HPP
#define VOXELDISPLAYLISTNODE_HPP

#include "cube.hpp"

namespace Anthrax
{

class VoxelDisplayListNode
{
public:
  VoxelDisplayListNode(Cube *cube, VoxelDisplayListNode *previous_cube, VoxelDisplayListNode *next_cube);
  VoxelDisplayListNode(Cube *cube) : VoxelDisplayListNode(cube, nullptr, nullptr) {}
  VoxelDisplayListNode() : VoxelDisplayListNode(nullptr, nullptr, nullptr) {}
  ~VoxelDisplayListNode();

  void setPrevious(VoxelDisplayListNode *new_previous_node);
  void setNext(VoxelDisplayListNode *new_next_node);
  VoxelDisplayListNode* next() const { return next_node_; }
  VoxelDisplayListNode* previous() const { return previous_node_; }
  Cube* getCubePtr() { return cube_; }
private:
  Cube *cube_;
  VoxelDisplayListNode *previous_node_;
  VoxelDisplayListNode *next_node_;
};

} // namespace Anthrax

#endif // VOXELDISPLAYLISTNODE_HPP
