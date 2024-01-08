/* ---------------------------------------------------------------- *\
 * voxelset.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-13
\* ---------------------------------------------------------------- */
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


VoxelSet::VoxelSet(int layer, std::vector<int> num_voxels, std::vector<uint16_t> voxel_type)
{
  layer_ = layer;
  num_counter_bytes_ = 4;
  num_voxels_ = num_voxels;
  voxel_type_ = voxel_type;
  is_uniform_ = (voxel_type_.size() == 1);
}


VoxelSet& VoxelSet::operator=(const VoxelSet& set)
{
  this->layer_ = set.layer_;
  this->num_counter_bytes_ = set.num_counter_bytes_;
  this->num_voxels_ = set.num_voxels_;
  this->voxel_type_ = set.voxel_type_;
  this->is_uniform_ = set.is_uniform_;
  return *this;
}


uint16_t VoxelSet::getVoxelType()
{
  // If this is a uniform set, return they type of voxel. Otherwise this could lead to unintended behavior.
  if (voxel_type_.size() == 0) return 0;
  return voxel_type_[0];
}

void VoxelSet::readFile(std::string input_filepath)
{
  std::ifstream file(input_filepath, std::ios::binary);

  if (!file)
  {
    // File doesn't yet exist
    generateAirFile(input_filepath);
    file.open(input_filepath, std::ios::binary);
  }
  char num_voxels_buffer[4];;
  char voxel_type_buffer[2];
  
  int num_voxels;
  uint16_t voxel_type;
  int counter = 0;
  while (!file.eof())
  {
    file.read(reinterpret_cast<char*>(&num_voxels), 4);
    if (file.eof()) break;
    file.read(reinterpret_cast<char*>(&voxel_type), 2);
    num_voxels_.push_back(num_voxels);
    voxel_type_.push_back(voxel_type);
  }
  file.close();
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
  int end = start + quadrant_set_length - 1;

  int index = 0;
  int counter = 0;
  std::vector<uint16_t> new_voxel_type  = std::vector<uint16_t>();
  std::vector<int> new_num_voxels = std::vector<int>();

  // Move to starting position
  while (counter < start)
  {
    int next_section_size = num_voxels_[index];
    if (next_section_size == 0)
    {
      index++;
      continue;
    }
    if (counter + next_section_size <= start)
    {
      index++;
      counter += next_section_size;
      continue;
    }
    if (counter + next_section_size > start)
    {
      int first_section_size = (counter + next_section_size - start);
      if (first_section_size >= quadrant_set_length)
      {
        new_num_voxels.push_back(quadrant_set_length);
        new_voxel_type.push_back(voxel_type_[index]);
        counter = start + first_section_size;
        break;
      }
      new_num_voxels.push_back(first_section_size);
      new_voxel_type.push_back(voxel_type_[index]);
      index++;
      counter += next_section_size;
      continue;
    }
  }

  // Iterate forwards until end position is reached
  while (counter < end + 1)
  {
    /*
    if (index > new_voxel_type.size())
    {
      // .zn file has incorrect number of elements
      index--;
      continue;
    }
    */
    int next_section_size = num_voxels_[index];
    if (next_section_size == 0)
    {
      index++;
      continue;
    }
    if (counter + next_section_size <= end + 1)
    {
      new_num_voxels.push_back(next_section_size);
      new_voxel_type.push_back(voxel_type_[index]);
      index++;
      counter += next_section_size;
      continue;
    }
    if (counter + next_section_size > end + 1)
    {
      new_num_voxels.push_back(end + 1 - counter);
      new_voxel_type.push_back(voxel_type_[index]);
      counter += next_section_size;
      continue;
    }
  }
  return VoxelSet(layer_ - 1, new_num_voxels, new_voxel_type);
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
