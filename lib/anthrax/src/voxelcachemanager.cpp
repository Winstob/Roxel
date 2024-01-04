/* ---------------------------------------------------------------- *\
 * voxelcachemanager.cpp
 * Author: Gavin Ralston
 * Date Created: 2024-01-03
\* ---------------------------------------------------------------- */

#include "voxelcachemanager.hpp"

namespace Anthrax
{

VoxelCacheManager::VoxelCacheManager()
{
}


VoxelCacheManager::~VoxelCacheManager()
{
  glDeleteVertexArrays(1, &voxel_vao_);
  glDeleteBuffers(1, &voxels_cache_);
}


void VoxelCacheManager::initialize(size_t cache_size)
{
  voxel_cache_size_ = cache_size;
  voxel_object_size_ = 2*sizeof(glm::vec3) + 3*sizeof(float) + 2*sizeof(int); // The size (in bytes) of all vertex attributes for a single voxel
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
}


void VoxelCacheManager::addCubes(Cube *new_cubes, int num_new_cubes)
{
  glBindBuffer(GL_ARRAY_BUFFER, voxels_cache_);
  for (unsigned int i = 0; i < num_new_cubes; i++)
  {
    Cube current_cube = *(new_cubes+i);

    // Get cube material settings
    glm::vec3 color = current_cube.getColor();
    GLfloat reflectivity = current_cube.getReflectivity();
    GLfloat shininess = current_cube.getShininess();
    GLfloat opacity = current_cube.getOpacity();

    /*
    // set cube position and scale
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, current_cube.getPosition());
    model = glm::scale(model, glm::vec3(current_cube.getSize()));
    */

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
}


void VoxelCacheManager::renderCubes()
{
  glBindVertexArray(voxel_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, voxels_cache_);
  
  glDrawArraysInstanced(GL_POINTS, 0, 1, voxel_cache_size_ / voxel_object_size_);

  glBindVertexArray(0);
}

} // namespace Anthrax