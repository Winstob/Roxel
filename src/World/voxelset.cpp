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

VoxelSet::VoxelSet(int total_num_voxels)
{
  total_num_voxels_ = total_num_voxels;
  //num_counter_bytes_ = ceil(3*layer_/8);
  num_counter_bytes_ = 4;
  num_voxels_ = std::vector<int>();
  voxel_type_ = std::vector<uint16_t>();
  is_uniform_ = false;
  calculateVoxelType();
}


VoxelSet::VoxelSet(int total_num_voxels, std::vector<int> num_voxels, std::vector<uint16_t> voxel_type)
{
  total_num_voxels_ = total_num_voxels;
  num_counter_bytes_ = 4;
  num_voxels_ = num_voxels;
  voxel_type_ = voxel_type;
  is_uniform_ = (voxel_type_.size() == 1);
  calculateVoxelType();
}


VoxelSet& VoxelSet::operator=(const VoxelSet& set)
{
  this->total_num_voxels_ = set.total_num_voxels_;
  this->num_counter_bytes_ = set.num_counter_bytes_;
  this->num_voxels_ = set.num_voxels_;
  this->voxel_type_ = set.voxel_type_;
  this->is_uniform_ = set.is_uniform_;
  this->calculateVoxelType();
  return *this;
}


void VoxelSet::calculateVoxelType()
{
  if (num_voxels_.size() == 0)
  {
    average_voxel_type_ = 0;
    return;
  }
  std::map<uint16_t, int> voxel_amounts;
  for (unsigned int i = 0; i < num_voxels_.size(); i++)
  {
    if (voxel_type_[i] != 0) voxel_amounts[voxel_type_[i]] += num_voxels_[i];
  }
  uint16_t largest_voxel_type = 0;
  int num_most_voxels = 0;
  for (std::map<uint16_t, int>::iterator itr = voxel_amounts.begin(); itr != voxel_amounts.end(); itr++)
  {
    if (itr->second > num_most_voxels)
    {
      num_most_voxels = itr->second;
      largest_voxel_type = itr->first;
    }
  }
  average_voxel_type_ = largest_voxel_type;
  if (average_voxel_type_ != 3 && average_voxel_type_ != 0) std::cout << average_voxel_type_ << std::endl;
}


uint16_t VoxelSet::getVoxelType()
{
  calculateVoxelType();
  return average_voxel_type_;
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
  calculateVoxelType();
}


VoxelSet VoxelSet::getQuadrant(int quadrant)
{
  //int set_length = 1 << (3*layer_); // 2^(3*layer_)
  int quadrant_set_length = total_num_voxels_ >> 3; // set_length/8 because there are 8 quadrants
  int start = quadrant * quadrant_set_length;
  int end = start + quadrant_set_length - 1;

  int index = 0;
  int counter = 0;
  std::vector<int> new_num_voxels = std::vector<int>();
  std::vector<uint16_t> new_voxel_type  = std::vector<uint16_t>();

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
  return VoxelSet(total_num_voxels_ >> 3, new_num_voxels, new_voxel_type);
}


void VoxelSet::bisect(VoxelSet *first, VoxelSet *second)
{
  //int set_length = 1 << (3*layer_); // 2^(3*layer_)
  int half_length = total_num_voxels_ >> 1;

  std::vector<int>::iterator num_vox_itr = num_voxels_.begin();
  std::vector<uint16_t>::iterator vox_type_itr = voxel_type_.begin();
  int counter = 0;

  while (counter < half_length)
  {
    int current_section_size = *num_vox_itr;
    if (!current_section_size == 0)
    {
      counter += current_section_size;
    }
    num_vox_itr++;
    vox_type_itr++;
  }

  std::vector<int> first_num_voxels(num_voxels_.begin(), num_vox_itr);
  std::vector<uint16_t> first_voxel_type(voxel_type_.begin(), vox_type_itr);
  std::vector<int> second_num_voxels(num_vox_itr, num_voxels_.end());
  std::vector<uint16_t> second_voxel_type(vox_type_itr, voxel_type_.end());
  if (half_length != counter)
  {
    first_num_voxels[first_num_voxels.size()-1] += half_length - counter;

    std::vector<int> tmp = { counter - half_length };
    second_num_voxels.insert(second_num_voxels.begin(), tmp.begin(), tmp.end());
    std::vector<uint16_t> tmp1 = { first_voxel_type[first_voxel_type.size()-1] };
    second_voxel_type.insert(second_voxel_type.begin(), tmp1.begin(), tmp1.end());
  }
  *first = VoxelSet(half_length, first_num_voxels, first_voxel_type);
  *second = VoxelSet(half_length, second_num_voxels, second_voxel_type);
  return;
}


void VoxelSet::bisectOld(VoxelSet *first, VoxelSet *second)
{
  //int set_length = 1 << (3*layer_); // 2^(3*layer_)
  int set_length = total_num_voxels_;
  int half_length = set_length >> 1;

  int index = 0;
  int counter = 0;
  std::vector<int> new_num_voxels = std::vector<int>();
  std::vector<uint16_t> new_voxel_type = std::vector<uint16_t>();
  int next_first_num_voxels = 0;
  uint16_t next_first_voxel_type = 0;


  while (counter < half_length)
  {
    int current_section_size = num_voxels_[index];
    if (current_section_size == 0)
    {
      index++;
      continue;
    }
    if (counter + current_section_size <= half_length)
    {
      new_num_voxels.push_back(current_section_size);
      new_voxel_type.push_back(voxel_type_[index]);
      index++;
      counter += current_section_size;
      continue;
    }
    if (counter + current_section_size > half_length)
    {
      new_num_voxels.push_back(half_length - counter);
      new_voxel_type.push_back(voxel_type_[index]);

      next_first_num_voxels = current_section_size + counter - half_length;
      next_first_voxel_type = voxel_type_[index];

      index++;
      counter += current_section_size;
      continue;
    }
  }
  *first = VoxelSet(total_num_voxels_ >> 1, new_num_voxels, new_voxel_type);

  //new_num_voxels = std::vector<int>();
  //new_voxel_type  = std::vector<uint16_t>();
  new_num_voxels.clear();
  new_voxel_type.clear();
  if (next_first_num_voxels != 0)
  {
    new_num_voxels.push_back(next_first_num_voxels);
    new_voxel_type.push_back(next_first_voxel_type);
  }

  while (counter < set_length)
  {
    int current_section_size = num_voxels_[index];
    if (current_section_size == 0)
    {
      index++;
      continue;
    }
    if (counter + current_section_size <= set_length)
    {
      new_num_voxels.push_back(current_section_size);
      new_voxel_type.push_back(voxel_type_[index]);
      index++;
      counter += current_section_size;
      continue;
    }
    if (counter + current_section_size > set_length)
    {
      new_num_voxels.push_back(set_length - counter);
      new_voxel_type.push_back(voxel_type_[index]);
      counter += current_section_size;
      continue;
    }
  }
  *second = VoxelSet(total_num_voxels_ >> 1, new_num_voxels, new_voxel_type);
}



void VoxelSet::generateAirFile(std::string filepath)
{
  std::ofstream file(filepath, std::ios::binary);
  uint32_t num_voxels = total_num_voxels_;
  uint16_t voxel_type = 0;

  file.write(reinterpret_cast<const char*>(&num_voxels), 4);
  file.write(reinterpret_cast<const char*>(&voxel_type), 2);
  file.close();
}
