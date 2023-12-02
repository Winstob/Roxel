#ifndef CUBE_HPP
#define CUBE_HPP

#include <array>

class Cube
{
  typedef float* Color;
  typedef int* Position;
private:
  Color color_;
  Position position_;
public:
  Cube()
  {
    color_ = reinterpret_cast<Color>(malloc(4*sizeof(float)));
    position_ = reinterpret_cast<Position>(malloc(3*sizeof(int)));
    color_[0] = 0.0;
    color_[1] = 0.0;
    color_[2] = 0.0;
    color_[3] = 0.0;
    position_[0] = 0;
    position_[1] = 0;
    position_[2] = 0;
  }


  Cube(std::array<float, 4> rgbk, std::array<int, 3> pos)
  {
    color_ = reinterpret_cast<Color>(malloc(4*sizeof(float)));
    position_ = reinterpret_cast<Position>(malloc(3*sizeof(int)));
    color_[0] = rgbk[0];
    color_[1] = rgbk[1];
    color_[2] = rgbk[2];
    color_[3] = rgbk[3];
    position_[0] = pos[0];
    position_[1] = pos[1];
    position_[2] = pos[2];
  }


  Cube(float *rgbk, int *pos)
  {
    color_ = reinterpret_cast<Color>(malloc(4*sizeof(float)));
    position_ = reinterpret_cast<Position>(malloc(3*sizeof(int)));
    color_ = rgbk;
    position_ = pos;
  }


  float* getColor()
  {
    return color_;
  }


  int* getPosition()
  {
    return position_;
  }
};
#endif // CUBE_HPP
