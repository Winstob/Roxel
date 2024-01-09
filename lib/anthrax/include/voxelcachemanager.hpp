/* ---------------------------------------------------------------- *\
 * voxelcachemanager.hpp
 * Author: Gavin Ralston
 * Date Created: 2024-01-03
\* ---------------------------------------------------------------- */
#ifndef VOXELCACHEMANAGER_HPP
#define VOXELCACHEMANAGER_HPP

#include "cube.hpp"
#include "voxeldisplaylist.hpp"
#include <vector>

#define KB(x) ((size_t) (x) << 10)
#define MB(x) ((size_t) (x) << 20)
#define GB(x) ((size_t) (x) << 30)


namespace Anthrax
{

class VoxelCacheManager
{
public:
  VoxelCacheManager();
  ~VoxelCacheManager();
  void initialize(size_t cache_size, bool (*cache_decision_function)(glm::vec3));
  void addCubes(Cube *new_cubes, int num_new_cubes);
  void addCube(std::weak_ptr<Cube> new_cube);
  void renderCubes();

  void updateCache();

  glm::vec3 view_position_; // This is temporary to test usage of the dynamic GPU cache
private:
  size_t voxel_cache_size_;
  size_t voxel_object_size_; // The size (in bytes) of all vertex attributes for a single voxel
  size_t max_num_voxels_; // The exact number of voxels that can be kept within the cache
  unsigned int voxel_vao_, voxels_cache_;

  int num_voxels_added_; // The amount of voxels added to the cache since the last frame - this is
                         // necessary to prevent overwriting recently added voxels. This should be
                         // refreshed only after the voxels are rearranged within the cache

  VoxelDisplayList voxel_display_list_;

  std::weak_ptr<Cube> *cache_emulator_;

  bool (*cacheDecisionFunction)(glm::vec3);
};

} // namespace Anthrax

#endif // VOXELCACHEMANAGER_HPP
