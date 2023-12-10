#include "player.hpp"
#include "window.hpp"
Player::Player()
{
  head_position_ = Anthrax::vec3<float>(0.0, 0.0, 0.0);
  left_direction_ = Anthrax::vec3<float>(-1.0, 0.0, 0.0);
  right_direction_ = Anthrax::vec3<float>(1.0, 0.0, 0.0);
  forward_direction_ = Anthrax::vec3<float>(0.0, 0.0, 1.0);
  back_direction_ = Anthrax::vec3<float>(0.0, 0.0, -1.0);
  up_direction_ = Anthrax::vec3<float>(0.0, 1.0, 0.0);
  down_direction_ = Anthrax::vec3<float>(0.0, -1.0, 0.0);
}


void Player::processInput()
{
  Anthrax::Window window = Anthrax::Window();
  if (window.getKeyPress(Anthrax::Key::W))
  {
    head_position_ = head_position_ + forward_direction_;
  }
  if (window.getKeyPress(Anthrax::Key::A))
  {
    head_position_ = head_position_ + left_direction_;
  }
  if (window.getKeyPress(Anthrax::Key::S))
  {
    head_position_ = head_position_ + back_direction_;
  }
  if (window.getKeyPress(Anthrax::Key::D))
  {
    head_position_ = head_position_ + right_direction_;
  }
  if (window.getKeyPress(Anthrax::Key::SPACE))
  {
    head_position_ = head_position_ + up_direction_;
  }
  if (window.getKeyPress(Anthrax::Key::LSHIFT))
  {
    head_position_ = head_position_ + down_direction_;
  }
  std::cout << "<" << head_position_.getX() << ", " << head_position_.getY() << ", " << head_position_.getZ() << ">" << std::endl;
}
