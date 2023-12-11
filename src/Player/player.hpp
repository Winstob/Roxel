#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "anthrax.hpp"

class Player
{
public:
  Player(Anthrax::Anthrax *anthrax_handle);
  void processInput();
private:
  Anthrax::Anthrax *anthrax_handle_;
  Anthrax::vec3<float> head_position_;
  Anthrax::vec4<float> head_rotation_;
  Anthrax::vec3<float> velocity_;
  Anthrax::vec3<float> left_direction_; // The direction to move in when moving left
  Anthrax::vec3<float> right_direction_; // The direction to move in when moving right
  Anthrax::vec3<float> forward_direction_; // The direction to move in when moving forwards
  Anthrax::vec3<float> back_direction_; // The direction to move in when moving back
  Anthrax::vec3<float> up_direction_; // The direction to move in when moving up
  Anthrax::vec3<float> down_direction_; // The direction to move in when moving down
  int speed_multiplier_;
  int jump_multiplier_;
  float fov_;
};
#endif // PLAYER_HPP
