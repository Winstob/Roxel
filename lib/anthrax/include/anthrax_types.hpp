#ifndef ANTHRAX_TYPES_HPP
#define ANTHRAX_TYPES_HPP

#include "glm/gtx/quaternion.hpp"
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
  vec3(T *array)
  {
    x_ = *array;
    y_ = *(array + 1*sizeof(T));
    z_ = *(array + 2*sizeof(T));
  }

  vec3<T>& operator=(const vec3& vec)
  {
    this->x_ = vec.x_;
    this->y_ = vec.y_;
    this->z_ = vec.z_;
    return *this;
  }
  vec3<T> operator+(const vec3& other_vec)
  {
    return vec3<T>(x_+other_vec.x_, y_+other_vec.y_, z_+other_vec.z_);
  }
  vec3<T> operator-()
  {
    return vec3<T>(-x_, -y_, -z_);
  }
  vec3<T>& operator+=(const vec3& other_vec)
  {
    *this = this->operator+(other_vec);
    return *this;
  }
  vec3<T> operator-(const vec3& other_vec)
  {
    return vec3<T>(x_-other_vec.x_, y_-other_vec.y_, z_-other_vec.z_);
  }
  vec3<T> operator*(const T& scalar)
  {
    return vec3<T>(x_*scalar, y_*scalar, z_*scalar);
  }
  vec3<T> operator/(const T& scalar)
  {
    return vec3<T>(x_/scalar, y_/scalar, z_/scalar);
  }

  T getX() const
  {
    return x_;
  }

  T getY() const
  {
    return y_;
  }

  T getZ() const
  {
    return z_;
  }

  glm::vec3 toGLM()
  {
    return glm::vec3(x_, y_, z_);
  }

  void setX(T x)
  {
    x_ = x;
  }

  void setY(T y)
  {
    y_ = y;
  }

  void setZ(T z)
  {
    z_ = z;
  }

  float getMagnitude()
  {
    return sqrt(x_*x_ + y_*y_ + z_*z_);
  }

  void normalize()
  {
    float magnitude = getMagnitude();
    if (magnitude > 0.0)
    {
      x_ = x_/magnitude;
      y_ = y_/magnitude;
      z_ = z_/magnitude;
    }
  }
  void normalizeHorizontals()
  {
    float magnitude = sqrt(abs(x_) + abs(z_));
    if (magnitude > 0.0)
    {
      x_ = x_/magnitude;
      z_ = z_/magnitude;
    }
  }
private:
  T x_;
  T y_;
  T z_;
};

template<class T>
T dot(vec3<T> u, vec3<T> v)
{
  return u.getX()*v.getX() + u.getY()*v.getY() + u.getZ()*v.getZ();
}

template<class T>
vec3<T> cross(vec3<T> u, vec3<T> v)
{
  T x = u.getY() * v.getZ() - u.getZ() * v.getY();
  T y = -(u.getX() * v.getZ() - u.getZ() * v.getX());
  T z = u.getX() * v.getY() - u.getY() * v.getX();
  return vec3<T>(x, y, z);
}


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

/*
class Quaternion
{
public:
  Quaternion()
  {
    w_ = 1.0f;
    x_ = 0.0f;
    y_ = 0.0f;
    z_ = 0.0f;
  }
  Quaternion(float w, float x, float y, float z)
  {
    w_ = w;
    x_ = x;
    y_ = y;
    z_ = z;
  }
  Quaternion& operator+(const Quaternion& other)
  {
    this->w_ += other.w_;
    this->x_ += other.x_;
    this->y_ += other.y_;
    this->z_ += other.z_;
    return *this;
  }
  Quaternion& operator*(const Quaternion& other)
  {
    float q0 = this->w_*other.w_ - this->x_*other.x_ - this->y_*other.y_ - this->z_*other.z_;
    float q1 = this->x_*other.w_ + this->w_*other.x_ - this->z_*other.y_ + this->y_*other.z_;
    float q2 = this->y_*other.w_ + this->z_*other.x_ + this->w_*other.y_ - this->x_*other.z_;
    float q3 = this->z_*other.w_ - this->y_*other.x_ + this->x_*other.y_ + this->w_*other.z_;
    this->w_ = q0;
    this->x_ = q1;
    this->y_ = q2;
    this->z_ = q3;
    return *this;
  }
  vec3<float> getForwardVector()
  {
    vec3<float> vec = vec3<float>(0.0, 0.0, 1.0);
    vec3<float> q_vec = vec3<float>(x_, y_, z_);
    return q_vec * (2.0f * dot(vec, q_vec))
      + (vec * (w_*w_ - dot(q_vec, q_vec)))
      + (cross(q_vec, vec) * 2.0f * w_);
  }
  vec3<float> getRightVector()
  {
    vec3<float> vec = vec3<float>(1.0, 0.0, 0.0);
    vec3<float> q_vec = vec3<float>(x_, y_, z_);
    return q_vec * (2.0f * dot(vec, q_vec))
      + (vec * (w_*w_ - dot(q_vec, q_vec)))
      + (cross(q_vec, vec) * 2.0f * w_);
  }
  vec3<float> getUpVector()
  {
    vec3<float> vec = vec3<float>(0.0, 1.0, 0.0);
    vec3<float> q_vec = vec3<float>(x_, y_, z_);
    return q_vec * (2.0f * dot(vec, q_vec))
      + (vec * (w_*w_ - dot(q_vec, q_vec)))
      + (cross(q_vec, vec) * 2.0f * w_);
  }
  float getW()
  {
    return w_;
  }
  float getX()
  {
    return x_;
  }
  float getY()
  {
    return y_;
  }
  float getZ()
  {
    return z_;
  }
private:
  float w_;
  float x_;
  float y_;
  float z_;
};
*/
class Quaternion
{
public:
  Quaternion()
  {
    q_ = glm::normalize(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  }
  Quaternion(float w, float x, float y, float z)
  {
    q_ = glm::normalize(glm::quat(w, x, y, z));
  }
  Quaternion(vec3<float> axis, float angle)
  {
    q_ = glm::normalize(glm::angleAxis(angle, glm::vec3(axis.getX(), axis.getY(), axis.getZ())));
  }
  Quaternion operator*(const Quaternion& other)
  {
    glm::quat q2 = glm::quat(other.getW(), other.getX(), other.getY(), other.getZ());
    glm::quat q3 = glm::normalize(this->q_*q2);
    return Quaternion(q3.w, q3.x, q3.y ,q3.z);
  }
  vec3<float> operator*(const vec3<float>& vec)
  {
    glm::vec3 rotated_vec = glm::normalize(q_*glm::vec3(vec.getX(), vec.getY(), vec.getZ()));
    return vec3<float>(rotated_vec.x, rotated_vec.y, rotated_vec.z);
  }
  vec3<float> getForwardVector() const
  {
    glm::vec3 forward = glm::vec3(0.0, 0.0, 1.0);
    forward = glm::normalize(q_*forward);
    return vec3<float>(forward.x, forward.y, forward.z);
  }
  vec3<float> getUpVector() const
  {
    glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
    up = glm::normalize(q_*up);
    return vec3<float>(up.x, up.y, up.z);
  }
  vec3<float> getRightVector() const
  {
    glm::vec3 right = glm::vec3(1.0, 0.0, 0.0);
    right = glm::normalize(q_*right);
    return vec3<float>(right.x, right.y, right.z);
  }
  float getW() const
  {
    return q_.w;
  }
  float getX() const
  {
    return q_.x;
  }
  float getY() const
  {
    return q_.y;
  }
  float getZ() const
  {
    return q_.z;
  }
 
private:
  glm::quat q_;
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
  LSHIFT,
  LCTRL
};

class MousePosition
{
public:
  MousePosition()
  {
    x_ = 0.0;
    y_ = 0.0;
  }
  float getX()
  {
    return x_;
  }
  float getY()
  {
    return y_;
  }
  void setX(float x)
  {
    x_ = x;
  }
  void setY(float y)
  {
    y_ = y;
  }
private:
  float x_;
  float y_;
};

} // namespace Anthrax
#endif // ANTHRAX_TYPES_HPP

