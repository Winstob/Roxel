/* ---------------------------------------------------------------- *\
 * voxelcachemanager.cpp
 * Author: Gavin Ralston
 * Date Created: 2024-01-03
\* ---------------------------------------------------------------- */

#include "voxelcachemanager.hpp"
#include <string.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Anthrax
{

VoxelCacheManager::VoxelCacheManager()
{
}


VoxelCacheManager::~VoxelCacheManager()
{
  // These will be cleared anyway by a call to glfwTerminate(), so technically not necessary
  glDeleteVertexArrays(1, &voxel_vao_);
  glDeleteBuffers(1, &voxels_cache_);

  free(cache_emulator_);
}


void VoxelCacheManager::initialize(size_t cache_size, bool (*cache_decision_function)(glm::vec3))
{
  cacheDecisionFunction = cache_decision_function;
  voxel_object_size_ = 2*sizeof(glm::vec3) + 3*sizeof(float) + 2*sizeof(int); // The size (in bytes) of all vertex attributes for a single voxel
  max_num_voxels_ = cache_size / voxel_object_size_;
  voxel_cache_size_ = max_num_voxels_ * voxel_object_size_;
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // Create vao to render voxels - no data is needed here as everything is computed in the geometry shader
  glGenVertexArrays(1, &voxel_vao_);
  glBindVertexArray(voxel_vao_);

  glGenBuffers(1, &voxels_cache_);
  glBindBuffer(GL_ARRAY_BUFFER, voxels_cache_);
  glBufferData(GL_ARRAY_BUFFER, voxel_cache_size_, NULL, GL_DYNAMIC_DRAW);
  // Fill voxel cache with 0's
  // Probably not the most effective way to do this, but it only happens once
  uint8_t zero = 0;
  for (unsigned int i = 0; i < voxel_cache_size_; i++)
  {
    glBufferSubData(GL_ARRAY_BUFFER, i, 1, &zero);
  }

  // Set up vertex attributes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribDivisor(0, 1);
  glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, voxel_object_size_, (void*)(sizeof(glm::vec3)));
  glEnableVertexAttribArray(1);
  glVertexAttribDivisor(1, 1);
  glVertexAttribPointer(2, 1, GL_UNSIGNED_INT, GL_FALSE, voxel_object_size_, (void*)(sizeof(glm::vec3)+sizeof(int)));
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)(sizeof(glm::vec3)+2*sizeof(int)));
  glEnableVertexAttribArray(3);
  glVertexAttribDivisor(3, 1);
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)(2*sizeof(glm::vec3)+2*sizeof(int)));
  glEnableVertexAttribArray(4);
  glVertexAttribDivisor(4, 1);
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)(2*sizeof(glm::vec3)+2*sizeof(int)+sizeof(float)));
  glEnableVertexAttribArray(5);
  glVertexAttribDivisor(5, 1);
  glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)(2*sizeof(glm::vec3)+2*sizeof(int)+2*sizeof(float)));
  glEnableVertexAttribArray(6);
  glVertexAttribDivisor(6, 1);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Set up CPU side cache emulator - this is to help control how to organize the GPU cache (removal and addition of voxels)
  cache_emulator_ = reinterpret_cast<std::weak_ptr<Cube>*>(calloc(sizeof(std::weak_ptr<Cube>), max_num_voxels_));

}


void VoxelCacheManager::addCubes(Cube *new_cubes, int num_new_cubes)
{
  /*
  glBindBuffer(GL_ARRAY_BUFFER, voxels_cache_);
  for (unsigned int i = 0; i < num_new_cubes; i++)
  {
    Cube current_cube = *(new_cubes+i);

    // Get cube material settings
    glm::vec3 color = current_cube.getColor();
    GLfloat reflectivity = current_cube.getReflectivity();
    GLfloat shininess = current_cube.getShininess();
    GLfloat opacity = current_cube.getOpacity();

    // set cube position and scale
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, current_cube.getPosition());
    model = glm::scale(model, glm::vec3(current_cube.getSize()));

    glm::vec3 position = current_cube.getPosition();
    GLint size = current_cube.getSize();

    GLuint render_faces = 0u;
    if (current_cube.render_face_[0]) render_faces |= 1u;
    if (current_cube.render_face_[1]) render_faces |= 2u;
    if (current_cube.render_face_[2]) render_faces |= 4u;
    if (current_cube.render_face_[3]) render_faces |= 8u;
    if (current_cube.render_face_[4]) render_faces |= 16u;
    if (current_cube.render_face_[5]) render_faces |= 32u;

    // Add this cube to the VBO
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_, sizeof(glm::vec3), &position);
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_ + sizeof(glm::vec3), sizeof(int), &size);
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_ + sizeof(glm::vec3) + sizeof(int), sizeof(int), &render_faces);

    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_ + sizeof(glm::vec3) + 2*sizeof(int), sizeof(glm::vec3), &color);
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_ + 2*sizeof(glm::vec3) + 2*sizeof(int), sizeof(float), &reflectivity);
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_ + 2*sizeof(glm::vec3) + 2*sizeof(int) + sizeof(float), sizeof(float), &shininess);
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_ + 2*sizeof(glm::vec3) + 2*sizeof(int) + 2*sizeof(float), sizeof(float), &opacity);
  }
  */
}


void VoxelCacheManager::addCube(std::weak_ptr<Cube> new_cube)
{
  voxel_display_list_.push_back(new_cube);
}


void VoxelCacheManager::renderCubes()
{
  glBindVertexArray(voxel_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, voxels_cache_);
  
  glDrawArraysInstanced(GL_POINTS, 0, 1, voxel_cache_size_ / voxel_object_size_);

  glBindVertexArray(0);
}


void VoxelCacheManager::updateCache()
{
  // Free up space by removing elements in the main VoxelDisplayList that have been deleted
  for (VoxelDisplayList::iterator itr = voxel_display_list_.begin(); itr != nullptr;)
  {
    if ((*itr).expired())
    {
      itr = voxel_display_list_.erase(itr);
    }
    else
    {
      itr++;
    }
  }
  // Iterate through the cache to find values that are no longer needed to make space for new ones
  std::vector<unsigned int> replaceable_cache_indices;
  for (unsigned int i = 0; i < max_num_voxels_; i++)
  {
    if (auto tmp = cache_emulator_[i].lock())
    {
      if (!cacheDecisionFunction(tmp->getPosition()))
      {
        replaceable_cache_indices.push_back(i);
      }
    }
    else
    {
      replaceable_cache_indices.push_back(i);
    }
  }
  
  glBindBuffer(GL_ARRAY_BUFFER, voxels_cache_);

  unsigned int num_voxels_added = 0;
  VoxelDisplayList::iterator itr = voxel_display_list_.begin();
  while (itr != nullptr && num_voxels_added < replaceable_cache_indices.size())
  {
    if (auto current_cube = (*itr).lock())
    {
      if (!(current_cube->is_in_cache_))
      {

        if (cacheDecisionFunction(current_cube->getPosition()))
        {
          // Only add this cube to the cache if it fits these criteria


          // Get cube material settings
          glm::vec3 color = current_cube->getColor();
          GLfloat reflectivity = current_cube->getReflectivity();
          GLfloat shininess = current_cube->getShininess();
          GLfloat opacity = current_cube->getOpacity();

          /*
          // set cube position and scale
          glm::mat4 model = glm::mat4(1.0f);
          model = glm::translate(model, current_cube.getPosition());
          model = glm::scale(model, glm::vec3(current_cube.getSize()));
          */

          glm::vec3 position = current_cube->getPosition();
          GLint size = current_cube->getSize();

          GLuint render_faces = 0u;
          if (current_cube->render_face_[0]) render_faces |= 1u;
          if (current_cube->render_face_[1]) render_faces |= 2u;
          if (current_cube->render_face_[2]) render_faces |= 4u;
          if (current_cube->render_face_[3]) render_faces |= 8u;
          if (current_cube->render_face_[4]) render_faces |= 16u;
          if (current_cube->render_face_[5]) render_faces |= 32u;

          // Add this cube to the VBO
          //int cache_location = max_num_voxels_ - num_voxels_added_ - 1;
          int cache_location = replaceable_cache_indices[num_voxels_added];
          glBufferSubData(GL_ARRAY_BUFFER, cache_location*voxel_object_size_, sizeof(glm::vec3), &position);
          glBufferSubData(GL_ARRAY_BUFFER, cache_location*voxel_object_size_ + sizeof(glm::vec3), sizeof(int), &size);
          glBufferSubData(GL_ARRAY_BUFFER, cache_location*voxel_object_size_ + sizeof(glm::vec3) + sizeof(int), sizeof(int), &render_faces);

          glBufferSubData(GL_ARRAY_BUFFER, cache_location*voxel_object_size_ + sizeof(glm::vec3) + 2*sizeof(int), sizeof(glm::vec3), &color);
          glBufferSubData(GL_ARRAY_BUFFER, cache_location*voxel_object_size_ + 2*sizeof(glm::vec3) + 2*sizeof(int), sizeof(float), &reflectivity);
          glBufferSubData(GL_ARRAY_BUFFER, cache_location*voxel_object_size_ + 2*sizeof(glm::vec3) + 2*sizeof(int) + sizeof(float), sizeof(float), &shininess);
          glBufferSubData(GL_ARRAY_BUFFER, cache_location*voxel_object_size_ + 2*sizeof(glm::vec3) + 2*sizeof(int) + 2*sizeof(float), sizeof(float), &opacity);

          // Now emulate this in the CPU cache emulator
          if (auto element_to_be_replaced = cache_emulator_[cache_location].lock())
          {
            element_to_be_replaced->is_in_cache_ = false;
          }
          cache_emulator_[cache_location] = current_cube;
          current_cube->is_in_cache_ = true;

          num_voxels_added++;
        }
      }
      itr++;
    }

    else
    {
      itr++;
      continue;
    }
  }
  // Remove all unused cache elements - they could be blocking smaller sub-cubes
  uint8_t tmp[voxel_object_size_] = {0};
  while (num_voxels_added < replaceable_cache_indices.size())
  {
    int cache_location = replaceable_cache_indices[num_voxels_added];
    glBufferSubData(GL_ARRAY_BUFFER, cache_location*voxel_object_size_, voxel_object_size_, &tmp);

    // Now emulate this in the CPU cache emulator
    if (auto element_to_be_replaced = cache_emulator_[cache_location].lock())
    {
      element_to_be_replaced->is_in_cache_ = false;
    }
    //cache_emulator_[cache_location] = nullptr;

    num_voxels_added++;

  }
}

} // namespace Anthrax
