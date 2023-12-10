#ifndef ANTHRAX_TYPES_HPP
#define ANTHRAX_TYPES_HPP

#include <string>
#include <iostream>

namespace Anthrax
{

template<class T>
class vec3
{
public:
  vec3()
  {
  };
  vec3(T x, T y, T z)
  {
    x_ = x;
    y_ = y;
    z_ = z;
  }
  vec3<T>& operator=(const vec3& vec)
  {
    this->x_ = vec.x_;
    this->y_ = vec.y_;
    this->z_ = vec.z_;
    return *this;
  }
  vec3<T>& operator+(const vec3& other_vec)
  {
    this->x_ += other_vec.x_;
    this->y_ += other_vec.y_;
    this->z_ += other_vec.z_;
    return *this;
  }

  T getX()
  {
    return x_;
  }

  T getY()
  {
    return y_;
  }

  T getZ()
  {
    return z_;
  }
private:
  T x_;
  T y_;
  T z_;
};


template<class T>
class vec4
{
public:
  vec4()
  {
  };
  vec4(T x, T y, T z, T w)
  {
    x_ = x;
    y_ = y;
    z_ = z;
    w_ = w;
  }
  vec4<T>& operator=(const vec4& vec)
  {
    this->x_ = vec.x_;
    this->y_ = vec.y_;
    this->z_ = vec.z_;
    this->w_ = vec.w_;
    return *this;
  }

  T getX()
  {
    return x_;
  }

  T getY()
  {
    return y_;
  }

  T getZ()
  {
    return z_;
  }

  T getW()
  {
    return w_;
  }

private:
  T x_;
  T y_;
  T z_;
  T w_;
};


enum Key
{
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  ONE,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE,
  ZERO,
  SPACE,
  LSHIFT
};

} // namespace Anthrax
#endif // ANTHRAX_TYPES_HPP

