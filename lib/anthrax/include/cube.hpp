#ifndef CUBE_HPP
#define CUBE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "anthrax_types.hpp"
#include <algorithm>

namespace Anthrax
{


class Cube
{
public:
  Cube()
  {
    type_id_ = 65535;

    ambient_ = glm::vec3(0.5f, 0.1f, 0.8f);
    diffuse_ = glm::vec3(0.5f, 0.1f, 0.8f);
    specular_ = glm::vec3(0.5f, 0.5f, 0.5f);
    shininess_ = 0.1;

    position_ = glm::vec3(0.0, 0.0, 0.0);
    size_ = 1;
  }

  Cube(vec3<float> pos, int size)
  {
    type_id_ = 65535;

    ambient_ = glm::vec3(0.5f, 0.1f, 0.8f); 
    diffuse_ = glm::vec3(0.5f, 0.1f, 0.8f); 
    specular_ = glm::vec3(0.5f, 0.5f, 0.5f);
    shininess_ = 0.1;
 
    position_ = pos.toGLM();
    size_ = size;
  }

  Cube(uint16_t type_id, vec3<float> position, int size, vec3<float> ambient, vec3<float> diffuse, vec3<float> specular, float shininess, float opacity)
  {
    type_id_ = type_id;
    position_ = position.toGLM();
    size_ = size;
    ambient_ = ambient.toGLM();
    diffuse_ = diffuse.toGLM();
    specular_ = specular.toGLM();
    shininess_ = shininess;
    opacity_ = opacity;
  }

  ~Cube()
  {
  }

  void setFaces(bool left, bool right, bool bottom, bool top, bool front, bool back)
  {
    render_face_[0] = left;
    render_face_[1] = right;
    render_face_[2] = bottom;
    render_face_[3] = top;
    render_face_[4] = front;
    render_face_[5] = back;
  }

  void setFaces(bool *faces)
  {
    for (unsigned int i = 0; i < 6; i++)
    {
      render_face_[i] = faces[i];
    }
  }

  glm::vec3 getAmbient()
  {
    return ambient_;
  }
  glm::vec3 getDiffuse()
  {
    return diffuse_;
  }
  glm::vec3 getSpecular()
  {
    return specular_;
  }
  float getShininess()
  {
    return shininess_;
  }
  float getOpacity()
  {
    return opacity_;
  }

  glm::vec3 getPosition()
  {
    return position_;
  }

  int getSize()
  {
    return size_;
  }

  uint16_t getTypeID()
  {
    return type_id_;
  }

  bool render_face_[6] = {false, false, false, false, false, false}; // Which faces to render: {left(-x normal), right(+x normal, bottom(-y normal, top(+y normal), front(-z normal), back(+z normal)}

private:
  uint16_t type_id_= 0;
  // Phong lighting properties
  glm::vec3 ambient_;
  glm::vec3 diffuse_;
  glm::vec3 specular_;
  float shininess_;

  float opacity_;
  glm::vec3 position_;
  int size_;

};

} // namespace Anthrax
#endif // CUBE_HPP
