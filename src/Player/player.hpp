/* ---------------------------------------------------------------- *\
 * player.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-10
\* ---------------------------------------------------------------- */
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "anthrax.hpp"
#include "playersettings.hpp"
#include "force.hpp"

class Player
{
public:
  Player(Anthrax::Anthrax *anthrax_handle);
  ~Player();
  Anthrax::vec3<float> getPosition() { return head_position_; }
  void processInput();
  bool updateForce(std::string name, Anthrax::vec3<float> vector);
  void update();
private:
  Anthrax::Anthrax *anthrax_handle_;
  Anthrax::vec3<float> head_position_;
  Anthrax::Quaternion head_rotation_;
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
  PlayerSettings *settings_;
  bool can_jump_;
  ForceMap *forces_;
};
#endif // PLAYER_HPP
