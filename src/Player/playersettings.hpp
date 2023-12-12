#ifndef PLAYERSETTINGS_HPP
#define PLAYERSETTINGS_HPP

#include "anthrax_types.hpp"

class PlayerSettings
{
public:
  PlayerSettings()
  {
    initDefaults();
  }
  void initDefaults()
  {
    noclip_ = false;
    hitbox_height_ = 64;
    hitbox_width_ = 16;
    hitbox_depth_ = 8;
    eye_height_ = hitbox_height_*5/5.5;
    fov_ = 70;
    mass_ = 1;
  }
  float getMass() const
  {
    return mass_;
  }
private:
  bool noclip_;
  float eye_height_;
  float hitbox_height_;
  float hitbox_width_;
  float hitbox_depth_;
  float fov_;
  float mass_;
};
#endif // PLAYERSETTINGS_HPP
