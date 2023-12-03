#ifndef CUBE_HPP
#define CUBE_HPP

#include "anthrax_types.hpp"

namespace Anthrax
{

class Cube
{
private:
  vec4<float> color_;
  vec3<int> position_;
public:
  Cube()
  {
    color_ = vec4<float>(0.0, 0.0, 0.0, 0.0);
    position_ = vec3<int>(0, 0, 0);
  }


  Cube(vec4<float> rgbk, vec3<int> pos)
  {
    color_ = rgbk;
    position_ = pos;
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

  int getPosX()
  {
    return position_.getX();
  }

  int getPosY()
  {
    return position_.getY();
  }

  int getPosZ()
  {
    return position_.getZ();
  }
};

} // namespace Anthrax
#endif // CUBE_HPP
