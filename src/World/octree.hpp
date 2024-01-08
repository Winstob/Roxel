/* ---------------------------------------------------------------- *\
 * octree.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-12
\* ---------------------------------------------------------------- */
#ifndef OCTREE_HPP
#define OCTREE_HPP

#include "anthrax_types.hpp"
#include "anthrax.hpp"
#include "voxelset.hpp"
#include "cube.hpp"
#include "cubeconvert.hpp"
#include <map>

class Octree
{
public:
  Octree() : Octree(0, 1) {}
  Octree(unsigned int layer, unsigned int file_layer) : Octree(layer, file_layer, "", Anthrax::vec3<int64_t>(0, 0, 0)) {}
  Octree(unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center);
  Octree(unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center, VoxelSet voxel_set);
  ~Octree();
  Octree* getChildPointer(int child) { return children_[child]; }
  void setCubeSettingsFile(std::string file);
  void setAnthraxPointer(Anthrax::Anthrax *anthrax_instance);
  void setLoadDecisionFunction(bool (*loadDecisionFunction)(uint64_t, int));
  void loadArea(Anthrax::vec3<int64_t> load_center, int load_distance);
  void getNewNeighbors();
  void setNeighbors(Octree **neighbors);
  void getCubes(std::vector<Anthrax::Cube> *cube_vector);
  bool isUniform() { return is_uniform_; }
  uint16_t getVoxelType() { return voxel_set_.getVoxelType(); }
  bool faceIsTransparent(uint8_t face) { return transparent_face_[face]; }
private:
  unsigned int layer_; // The location of this layer - layer 0 will always be a leaf 
  unsigned int file_layer_; // The layer at which files need to be read in
  VoxelSet voxel_set_; // Container for voxel data
  std::string path_; // The path to get from the top layer to this one - formatted as "[0-7]*"
  bool is_uniform_; // True if all voxels in all subtrees are of the same type
  bool is_leaf_; // True if all voxels in all subtrees are of the same type
  Octree *children_[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}; // Pointers to the children
  Octree *neighbors_[6] = {NULL, NULL, NULL, NULL, NULL, NULL}; // Pointers to neighbors
  Anthrax::vec3<int64_t> center_; // The center of the octree - used to find the quadrant of any given location
  bool transparent_face_[6] = {true, true, true, true, true, true}; // List of which faces are partially or completely transparent - any adjacent faces on adjacent blocks must be drawn. This list matches inversely to Anthrax::Cube::render_face_ variables to avoid extra calculations, so the list goes in order as follows: {right(+x normal), left(-x normal, top(+y normal, bottom(-y normal), back(+z normal), front(-z normal)}
  
  Anthrax::Cube *cube_pointer_;

  static CubeConvert cube_converter_;
  static Anthrax::Anthrax *anthrax_instance_;
  static bool (*loadDecisionFunction)(uint64_t, int);
};
#endif // OCTREE_HPP
