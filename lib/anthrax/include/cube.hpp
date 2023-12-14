#ifndef CUBE_HPP
#define CUBE_HPP

#include "anthrax_types.hpp"

namespace Anthrax
{

class Cube
{
private:
  vec4<float> color_;
  vec3<float> position_;
  int size_;
public:
  Cube()
  {
    color_ = vec4<float>(0.0, 0.0, 0.0, 0.0);
    position_ = vec3<float>(0.0, 0.0, 0.0);
    size_ = 1;
  }

  Cube(vec4<float> rgbk, vec3<float> pos)
  {
    color_ = rgbk;
    position_ = pos;
    size_ = 1;
  }

  Cube(vec4<float> rgbk, vec3<float> pos, int size)
  {
    color_ = rgbk;
    position_ = pos;
    size_ = size;
  }


  float getColorR()
  {
    return color_.getX();
  }

  float getColorG()
  {
    return color_.getY();
  }

  float getColorB()
  {
    return color_.getZ();
  }

  float getColorK()
  {
    return color_.getW();
  }

  float getPosX()
  {
    return position_.getX();
  }

  float getPosY()
  {
    return position_.getY();
  }

  float getPosZ()
  {
    return position_.getZ();
  }

  int getSize()
  {
    return size_;
  }
};

} // namespace Anthrax
#endif // CUBE_HPP
