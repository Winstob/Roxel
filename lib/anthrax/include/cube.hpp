#ifndef CUBE_HPP
#define CUBE_HPP

#include "anthrax_types.hpp"

namespace Anthrax
{


class Cube
{
private:
  // Phong lighting properties
  glm::vec3 ambient_;
  glm::vec3 diffuse_;
  glm::vec3 specular_;
  float shininess_;
  float opacity_;
  glm::vec3 position_;
  int size_;
  bool render_face_[6]; // Which faces to render: {left(-x normal), right(+x normal, bottom(-y normal, top(+y normal), front(-z normal), back(+z normal)}
public:
  Cube()
  {
    ambient_ = glm::vec3(0.5f, 0.1f, 0.8f);
    diffuse_ = glm::vec3(0.5f, 0.1f, 0.8f);
    specular_ = glm::vec3(0.5f, 0.5f, 0.5f);
    shininess_ = 0.1;

    position_ = glm::vec3(0.0, 0.0, 0.0);
    size_ = 1;
  }

  Cube(vec3<float> pos, int size)
  {
    ambient_ = glm::vec3(0.5f, 0.1f, 0.8f); 
    diffuse_ = glm::vec3(0.5f, 0.1f, 0.8f); 
    specular_ = glm::vec3(0.5f, 0.5f, 0.5f);
    shininess_ = 0.1;
 
    position_ = pos.toGLM();
    size_ = size;
  }

  Cube(vec3<float> position, int size, vec3<float> ambient, vec3<float> diffuse, vec3<float> specular, float shininess, float opacity)
  {
    position_ = position.toGLM();
    size_ = size;
    ambient_ = ambient.toGLM();
    diffuse_ = diffuse.toGLM();
    specular_ = specular.toGLM();
    shininess_ = shininess;
    opacity_ = opacity;
  }

  void setFaces(bool front, bool back, bool left, bool right, bool top, bool bottom)
  {
    render_face_[0] = front;
    render_face_[1] = back;
    render_face_[2] = left;
    render_face_[3] = right;
    render_face_[4] = top;
    render_face_[5] = bottom;
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
};

} // namespace Anthrax
#endif // CUBE_HPP
