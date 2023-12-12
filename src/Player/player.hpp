#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "anthrax.hpp"

class Player
{
public:
  Player(Anthrax::Anthrax *anthrax_handle);
  ~Player();
  void processInput();
  void update();
private:
  Anthrax::Anthrax *anthrax_handle_;
  Anthrax::vec3<float> head_position_;
  Anthrax::Quaternion head_rotation_;
  Anthrax::Quaternion head_rotation_yaw_;
  Anthrax::Quaternion head_rotation_pitch_;
  Anthrax::Quaternion head_rotation_roll_;
  Anthrax::vec3<float> velocity_;
  Anthrax::vec3<float> left_direction_; // The direction to move in when moving left
  Anthrax::vec3<float> right_direction_; // The direction to move in when moving right
  Anthrax::vec3<float> forward_direction_; // The direction to move in when moving forwards
  Anthrax::vec3<float> back_direction_; // The direction to move in when moving back
  Anthrax::vec3<float> up_direction_; // The direction to move in when moving up
  Anthrax::vec3<float> down_direction_; // The direction to move in when moving down
  Anthrax::MousePosition *mouse_pos_;
  Anthrax::MousePosition prev_mouse_pos_;
  bool mouse_was_paused_ = true;
  bool mouse_paused_ = false;
  float sensitivity_;
  int speed_multiplier_;
  int jump_multiplier_;
  float fov_;
  Anthrax::vec3<float> x_direction_;
  Anthrax::vec3<float> y_direction_;
  Anthrax::vec3<float> z_direction_;
};
#endif // PLAYER_HPP
