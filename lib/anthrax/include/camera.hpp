/* ---------------------------------------------------------------- *\
 * camera.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-11-24
\* ---------------------------------------------------------------- */


#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

namespace Anthrax
{

// Default camera values
const float ZOOM        =  45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
  // camera Attributes
  glm::vec3 position_;
  glm::quat rotation_;
  // camera options
  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;

  // constructor with vectors
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::quat rotation = glm::quat(0.0f, 0.0, 0.0f, 0.0f)) : Zoom(ZOOM)
  {
    position_ = position;
    rotation_ = rotation;
  }
  // constructor with scalar values
  Camera(float posX, float posY, float posZ, float rotW, float rotX, float rotY, float rotZ) : Zoom(ZOOM)
  {
    position_ = glm::vec3(posX, posY, posZ);
    rotation_ = glm::quat(rotW, rotX, rotY, rotZ);
  }

  // returns the view matrix calculated using Euler Angles and the LookAt Matrix
  glm::mat4 GetViewMatrix()
  {
    glm::vec3 forward = rotation_ * glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 up = rotation_ * glm::vec3(0.0f, 1.0f, 0.0f);
    return glm::lookAt(position_, position_ + forward, up);
  }

  glm::vec3 getLookDirection()
  {
    return rotation_ * glm::vec3(0.0f, 0.0f, 1.0f);
  }

  // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
  void ProcessMouseScroll(float yoffset)
  {
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
      Zoom = 1.0f;
    if (Zoom > 45.0f)
      Zoom = 45.0f;
  }
  void setPosition(glm::vec3 position)
  {
    position_ = position;
  }

  void setRotation(glm::quat rotation)
  {
    rotation_ = rotation;
  }


private:
};

} // namespace Anthrax

#endif // CAMERA_HPP

