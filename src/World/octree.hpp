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

class Octree : public std::enable_shared_from_this<Octree>
{
public:
  Octree() : Octree(std::weak_ptr<Octree>(), 0, 1) {}
  Octree(std::weak_ptr<Octree> parent, unsigned int layer, unsigned int file_layer) : Octree(parent, layer, file_layer, "", Anthrax::vec3<int64_t>(0, 0, 0)) {}
  Octree(std::weak_ptr<Octree> parent, unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center);
  Octree(std::weak_ptr<Octree> parent, unsigned int layer, unsigned int file_layer, std::string path, Anthrax::vec3<int64_t> center, VoxelSet voxel_set);
  ~Octree();
  unsigned int getLayer() const { return layer_; }
  std::weak_ptr<Octree> getParentPointer() { return parent_; }
  std::weak_ptr<Octree> getChildPointer(int child) { return children_[child]; }
  void splitVoxelSet();
  void setCubeSettingsFile(std::string file);
  void setAnthraxPointer(Anthrax::Anthrax *anthrax_instance);
  void setLoadDecisionFunction(bool (*loadDecisionFunction)(uint64_t, int));
  void loadChildren();
  void deleteChildren();
  void loadAreaRecursive(Anthrax::vec3<int64_t> load_center);
  void getNewNeighbors();
  void setNeighbors(std::weak_ptr<Octree> *neighbors);
  void getCubes();
  Anthrax::vec3<int64_t> getCenter() const { return center_; }
  bool isUniform() { return is_uniform_; }
  bool isLeaf() { return is_leaf_; }
  uint16_t getVoxelType() { return voxel_set_.getVoxelType(); }
  bool faceIsTransparent(uint8_t face) { return transparent_face_[face]; }

  bool neighbors_changed_ = false;
  static bool (*loadDecisionFunction)(uint64_t, int);
  bool parent_load_checked = false; // Only for use in World
private:
  unsigned int layer_; // The location of this layer - layer 0 will always be a leaf 
  unsigned int file_layer_; // The layer at which files need to be read in
  VoxelSet voxel_set_; // Container for voxel data
  VoxelSet voxel_set_quadrants_[8];
  std::string path_; // The path to get from the top layer to this one - formatted as "[0-7]*"
  bool is_uniform_; // True if all voxels in all subtrees are of the same type
  bool is_leaf_; // True if this octree has no children
  std::weak_ptr<Octree> parent_;
  std::shared_ptr<Octree> children_[8];
  std::weak_ptr<Octree> neighbors_[6];
  Anthrax::vec3<int64_t> center_; // The center of the octree - used to find the quadrant of any given location
  bool transparent_face_[6] = {true, true, true, true, true, true}; // List of which faces are partially or completely transparent - any adjacent faces on adjacent blocks must be drawn. This list matches inversely to Anthrax::Cube::render_face_ variables to avoid extra calculations, so the list goes in order as follows: {right(+x normal), left(-x normal, top(+y normal, bottom(-y normal), back(+z normal), front(-z normal)}
  
  std::shared_ptr<Anthrax::Cube> cube_pointer_;

  static CubeConvert cube_converter_;
  static Anthrax::Anthrax *anthrax_instance_;
};
#endif // OCTREE_HPP
