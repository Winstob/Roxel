#include "voxelset.hpp"

#include <fstream>
#include <cstring>
#include <cmath>
#include <iostream>

VoxelSet::VoxelSet(int layer)
{
  layer_ = layer;
  //num_counter_bytes_ = ceil(3*layer_/8);
  num_counter_bytes_ = 4;
  voxel_type_  = std::vector<uint16_t>();
  num_voxels_ = std::vector<int>();
  is_uniform_ = false;
}


VoxelSet::VoxelSet(std::vector<int> num_voxels, std::vector<uint16_t> voxel_type)
{
  layer_ = 0;
  num_counter_bytes_ = 0;
  num_voxels_ = num_voxels;
  voxel_type_ = voxel_type;
  is_uniform_ = (voxel_type_.size() == 1);
}


void VoxelSet::readFile(std::string input_filepath)
{
  std::filesystem::path filepath{input_filepath};
  if (!std::filesystem::exists(filepath))
  {
    // File doesn't yet exist
    generateAirFile(input_filepath);
  }
  if (std::filesystem::file_size(filepath) == 0)
  {
    // File is empty
    generateAirFile(input_filepath);
  }
  char num_voxels_buffer[num_counter_bytes_];;
  char voxel_type_buffer[2];
  
  std::ifstream file(input_filepath);//, std::ios::binary);
  int bytes_read = 0;
  int num_voxels;
  uint16_t voxel_type;
  while (file.good())
  {
    file.read(num_voxels_buffer, num_counter_bytes_);
    file.read(voxel_type_buffer, 2);
    // Assemble num_voxels
    memcpy(&num_voxels, num_voxels_buffer, num_counter_bytes_);
    // Assemble voxel_type
    memcpy(&voxel_type, voxel_type_buffer, 2);
    num_voxels_.push_back(num_voxels);
    voxel_type_.push_back(voxel_type);
  }
  if (voxel_type_.size() == 1)
  {
    is_uniform_ = true;
  }
}


VoxelSet VoxelSet::getQuadrant(int quadrant)
{
  int set_length = 1 << (3*layer_); // 2^(3*layer_)
  int quadrant_set_length = set_length >> 3; // set_length/8 because there are 8 quadrants
  int start = quadrant * quadrant_set_length;
  int end = start + quadrant_set_length;

  int index = 0;
  int counter = 0;
  while (true)
  {
    int next_section_size = num_voxels_[index];
    if (counter >= start)
    {
      break;
    }
    counter += next_section_size;
    if (counter < start)
    {
      index++;
    }
  }

  std::vector<uint16_t> new_voxel_type  = std::vector<uint16_t>();
  std::vector<int> new_num_voxels = std::vector<int>();
  while (start < end)
  {
    int next_section_size = num_voxels_[index];
    if (counter + next_section_size < end)
    {
      new_num_voxels.push_back(next_section_size);
      new_voxel_type.push_back(voxel_type_[index]);
      counter += next_section_size;
      continue;
    }

  }
  return VoxelSet(new_num_voxels, new_voxel_type);
}


void VoxelSet::generateAirFile(std::string filepath)
{
  std::ofstream file(filepath, std::ios::binary);
  uint32_t num_voxels = 1 << (3*layer_);
  uint16_t voxel_type = 0;

  file.write(reinterpret_cast<const char*>(&num_voxels), 4);
  file.write(reinterpret_cast<const char*>(&voxel_type), 2);
  file.close();
}
