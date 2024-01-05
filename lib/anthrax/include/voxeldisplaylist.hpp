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
  size_t size() { return size_; }
  void push_back(Cube *new_cube);
  void push_front(Cube *new_cube);

  class iterator
  {
  public:
    iterator(VoxelDisplayListNode *node = nullptr) : node_(node) {}
    Cube* operator*() { return node_->getCubePtr(); }
    iterator& operator++()
    {
      node_ = node_->next();
      return *this;
    }
    iterator operator++(int)
    {
      iterator tmp(*this);
      node_ = node_->next();
      return tmp;
    }
    iterator& operator--()
    {
      node_ = node_->previous();
      return *this;
    }
    iterator operator--(int)
    {
      iterator tmp(*this);
      node_ = node_->previous();
      return tmp;
    }
    bool operator==(const iterator& r) const { return node_ == r.node_; }
    bool operator!=(const iterator& r) const { return node_ != r.node_; }

  private:
    VoxelDisplayListNode *node_;
  };

  iterator begin() { return iterator(front_); }

private:
  VoxelDisplayListNode *front_;
  VoxelDisplayListNode *back_;
  size_t size_;
};


} // namespace Anthrax

#endif // VOXELDISPLAYLIST_HPP


