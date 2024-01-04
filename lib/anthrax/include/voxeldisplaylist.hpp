/* ---------------------------------------------------------------- *\
 * voxeldisplaylist.hpp
 * Author: Gavin Ralston
 * Date Created: 2024-01-04
\* ---------------------------------------------------------------- */

#ifndef VOXELDISPLAYLIST_HPP
#define VOXELDISPLAYLIST_HPP

#include "cube.hpp"
#include "voxeldisplaylistnode.hpp"

namespace Anthrax
{


class VoxelDisplayList
{
public:
  VoxelDisplayList();
  ~VoxelDisplayList();
  void push_back(Cube *new_cube);
  void push_front(Cube *new_cube);
private:
  VoxelDisplayListNode *front_;
  VoxelDisplayListNode *back_;
};


} // namespace Anthrax

#endif // VOXELDISPLAYLIST_HPP


