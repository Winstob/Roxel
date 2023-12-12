/* ---------------------------------------------------------------- *\
 * force.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-12
\* ---------------------------------------------------------------- */
#ifndef FORCE_HPP
#define FORCE_HPP

#include <string>
#include "anthrax_types.hpp"


class Force
{
public:
  Force(std::string name, Anthrax::vec3<float> vector)
  {
    name_ = name;
    force_vector_ = vector;
  }

  std::string getName()
  {
    return name_;
  }

  Anthrax::vec3<float> getVector()
  {
    return force_vector_;
  }

  void setVector(Anthrax::vec3<float> vector)
  {
    force_vector_ = vector;
  }

private:
  std::string name_;
  Anthrax::vec3<float> force_vector_;
};


class ForceMap
{
public:
  ForceMap()
  {
    force_map_ = std::vector<Force>();
  }

  bool update(std::string name, Anthrax::vec3<float> vector)
  {
    return update(Force(name, vector));
  }

  bool update(Force force)
  {
    for (unsigned int i = 0; i < force_map_.size(); i++)
    {
      if (force_map_[i].getName() == force.getName())
      {
        // This is the desired force - update it accordingly
        force_map_[i].setVector(force.getVector());
        return true;
      }
    }
    // If the force is not found, add it
    force_map_.push_back(force);
    return false;
  }

  bool remove(std::string name)
  {
    for (unsigned int i = 0; i < force_map_.size(); i++)
    {
      if (force_map_[i].getName() == name)
      {
        // Force found - remove it
        force_map_.erase(force_map_.begin() + i);
        return true;
      }
    }
    return false;
  }
 
  Anthrax::vec3<float> netForce()
  {
    Anthrax::vec3<float> net_force = Anthrax::vec3<float>(0.0, 0.0, 0.0);
    for (unsigned int i = 0; i < force_map_.size(); i++)
    {
      net_force += force_map_[i].getVector();
    }
    return net_force;
  }

private:
  std::vector<Force> force_map_;
};

#endif // FORCE_HPP
