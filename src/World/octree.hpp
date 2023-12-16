/* ---------------------------------------------------------------- *\
 * octree.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-12
\* ---------------------------------------------------------------- */
#ifndef OCTREE_HPP
#define OCTREE_HPP

#include "anthrax_types.hpp"
#include "voxelset.hpp"
#include "cube.hpp"
#include "cubeconvert.hpp"

class Octree
{
public:
  Octree();
  Octree(unsigned int layer, unsigned int file_layer);
  Octree(unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center);
  Octree(unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center, VoxelSet voxel_set);
  ~Octree();
  void setCubeSettingsFile(std::string file);
  void loadArea(Anthrax::vec3<int64_t> center, int load_distance);
  void getCubes(std::vector<Anthrax::Cube> *cube_vector);
private:
  unsigned int layer_; // The location of this layer - layer 0 will always be a leaf 
  unsigned int file_layer_; // The layer at which files need to be read in
  VoxelSet voxel_set_; // Container for voxel data
  std::string path_; // The path to get from the top layer to this one - formatted as "[0-7]*"
  bool is_leaf_;
  bool is_uniform_; // True if all voxels in all subtrees are of the same type
  Octree *children_[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}; // Pointers to the children
  Anthrax::vec3<int64_t> center_; // The center of the octree - used to find the quadrant of any given location
  static CubeConvert cube_converter_;
};
#endif // OCTREE_HPP
