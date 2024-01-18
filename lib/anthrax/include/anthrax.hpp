/* ---------------------------------------------------------------- *\
 * anthrax.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-10
\* ---------------------------------------------------------------- */


#ifndef ANTHRAX_HPP
#define ANTHRAX_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shader.hpp"
#include "camera.hpp"

#include "cube.hpp"
#include "voxeldisplaylist.hpp"
#include "voxelcachemanager.hpp"

#include "anthrax_types.hpp"
#include <vector>
#include <map>

namespace Anthrax
{

class Anthrax
{
public:
  Anthrax();
  static bool getKeyPress(Key key);
  void getMousePosition(MousePosition *mouse_pos);
  float getTimeSinceLastFrame() const;

  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
  static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
  static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void processInput(GLFWwindow *window);

  int startWindow();
  int renderFrame();

  void addVoxel(std::weak_ptr<Cube> cube);
  void setCameraPosition(vec3<float> position);
  void setCameraRotation(Quaternion rotation);

  enum RenderType
  {
    NO_SHADOWS,
    DYNAMIC_SHADOWS,
    RAYTRACED
  };

  //std::map<uint16_t, std::vector<Cube>> voxel_buffer_map_;

private:
  void renderScene();
  void gBufferSetup();
  void ssaoFramebufferSetup();
  void ssaoKernelSetup();
  void renderQuad();
  void resizeWindow();

  RenderType render_type_;

  std::vector<Cube> voxel_buffer_;

  static GLFWwindow* window;

  unsigned int framebuffer_, texture_color_buffer_, renderbuffer_object_;
  unsigned int g_buffer_ = 0, g_position_texture_ = 0, g_normal_texture_ = 0, g_color_texture_ = 0, g_material_texture_ = 0, g_depth_rbo_ = 0;
  unsigned int ssao_framebuffer_ = 0, ssao_texture_ = 0, ssao_noise_texture_ = 0;
  unsigned int quad_vao_ = 0, quad_vbo_ = 0;
  Shader* lighting_pass_shader_ = nullptr;
  Shader* ssao_pass_shader_ = nullptr;
  Shader* geometry_pass_shader_ = nullptr;
  Shader* cube_shader_ = nullptr;
  Shader* screen_shader_ = nullptr;

  VoxelCacheManager* voxel_cache_manager_;

  // settings
  static unsigned int window_width_;
  static unsigned int window_height_;
  static unsigned int render_distance_;

  // camera
  static Camera camera;
  static float mouse_x_;
  static float mouse_y_;

  // timing
  static float deltaTime;	// time between current frame and last frame
  static float lastFrame;

  static bool wireframe_mode_;
  static bool window_size_changed_;

};

} // namespace Anthrax
#endif // ANTHRAX_HPP
