#include "player.hpp"

Player::Player(Anthrax::Anthrax *anthrax_handle)
{
  anthrax_handle_ = anthrax_handle;
  x_direction_ = Anthrax::vec3<float>(1.0, 0.0, 0.0);
  y_direction_ = Anthrax::vec3<float>(0.0, 1.0, 0.0);
  z_direction_ = Anthrax::vec3<float>(0.0, 0.0, 1.0);
  head_rotation_ = Anthrax::Quaternion(1.0, 0.0, 0.0, 0.0);
  head_position_ = Anthrax::vec3<float>(0.0, 0.0, 0.0);
  left_direction_ = Anthrax::vec3<float>(-1.0, 0.0, 0.0);
  right_direction_ = Anthrax::vec3<float>(1.0, 0.0, 0.0);
  forward_direction_ = Anthrax::vec3<float>(0.0, 0.0, 1.0);
  back_direction_ = Anthrax::vec3<float>(0.0, 0.0, -1.0);
  up_direction_ = Anthrax::vec3<float>(0.0, 1.0, 0.0);
  down_direction_ = Anthrax::vec3<float>(0.0, -1.0, 0.0);
  mouse_pos_ = new Anthrax::MousePosition();
  prev_mouse_pos_ = Anthrax::MousePosition();
  sensitivity_ = 0.01;
}


Player::~Player()
{
  delete(mouse_pos_);
}


void Player::processInput()
{
  bool roll = false;
  // Keyboard Inputs
  float move_multiplier = 1.0;
  Anthrax::vec3<float> move_vector(0.0, 0.0, 0.0);
  if (anthrax_handle_->getKeyPress(Anthrax::Key::W))
  {
    move_vector += forward_direction_;
  }
  if (anthrax_handle_->getKeyPress(Anthrax::Key::A))
  {
    move_vector += left_direction_;
  }
  if (anthrax_handle_->getKeyPress(Anthrax::Key::S))
  {
    move_vector += back_direction_;
  }
  if (anthrax_handle_->getKeyPress(Anthrax::Key::D))
  {
    move_vector += right_direction_;
  }
  if (anthrax_handle_->getKeyPress(Anthrax::Key::SPACE))
  {
    move_vector += up_direction_;
  }
  if (anthrax_handle_->getKeyPress(Anthrax::Key::LSHIFT))
  {
    move_vector += down_direction_;
  }
  if (anthrax_handle_->getKeyPress(Anthrax::Key::LCTRL))
    roll = true;
  move_vector.normalizeHorizontals();
  move_vector = move_vector*move_multiplier;
  head_position_ += move_vector;
  std::cout << "<" << head_position_.getX() << ", " << head_position_.getY() << ", " << head_position_.getZ() << ">" << std::endl;

  // Mouse Inputs
  if (!mouse_paused_)
  {
    anthrax_handle_->getMousePosition(mouse_pos_);
    if (mouse_was_paused_)
    {
      prev_mouse_pos_.setX(mouse_pos_->getX());
      prev_mouse_pos_.setY(mouse_pos_->getY());
    }
    float mouse_x = sensitivity_*(mouse_pos_->getX() - prev_mouse_pos_.getX());
    float mouse_y = sensitivity_*(mouse_pos_->getY() - prev_mouse_pos_.getY());
    prev_mouse_pos_.setX(mouse_pos_->getX());
    prev_mouse_pos_.setY(mouse_pos_->getY());
    std::cout << mouse_y << std::endl;

    Anthrax::Quaternion yaw_quaternion = Anthrax::Quaternion(up_direction_, -mouse_x);
    Anthrax::Quaternion pitch_quaternion = Anthrax::Quaternion(head_rotation_.getRightVector(), mouse_y);
    Anthrax::Quaternion roll_quaternion = Anthrax::Quaternion(head_rotation_.getForwardVector(), mouse_x);
    if (!roll)
    {
      head_rotation_ = pitch_quaternion * head_rotation_;
      head_rotation_ = yaw_quaternion * head_rotation_;
    }
    else
    {
      head_rotation_ = pitch_quaternion * head_rotation_;
      head_rotation_ = roll_quaternion * head_rotation_;
      up_direction_ = roll_quaternion * up_direction_;
    }

    right_direction_ = -(head_rotation_.getRightVector());
    left_direction_ = -right_direction_;
    forward_direction_ = -Anthrax::cross(right_direction_, up_direction_);
    back_direction_ = -forward_direction_;
  }
  mouse_was_paused_ = mouse_paused_;
  
}


void Player::update()
{
  anthrax_handle_->setCameraPosition(head_position_);
  anthrax_handle_->setCameraRotation(head_rotation_);
}
