/* ---------------------------------------------------------------- *\
 * voxelset.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-13
\* ---------------------------------------------------------------- */


#ifndef VOXELSET_HPP
#define VOXELSET_HPP

#include <vector>
#include <string>
#include <filesystem>
#include <map>

class VoxelSet
{
public:
  VoxelSet() { VoxelSet(0); }
  VoxelSet(int layer);
  VoxelSet(int layer, std::vector<int> num_voxels, std::vector<uint16_t> voxel_type);
  VoxelSet& operator=(const VoxelSet& set);
  void calculateVoxelType();
  uint16_t getVoxelType();
  void readFile(std::string filepath);
  bool isUniform() { return is_uniform_; }
  VoxelSet getQuadrant(int quadrant);
private:
  int layer_;
  int num_counter_bytes_;
  std::vector<uint16_t> voxel_type_; // Stores the type of voxel at a certain index
  std::vector<int> num_voxels_; // Stores the number of same-type voxels in order (more info in world.hpp)
  bool is_uniform_;
  uint16_t average_voxel_type_;

  void generateAirFile(std::string filepath);
};
#endif // VOXELSET_HPP
