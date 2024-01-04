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

#include "shader.hpp"
#include "camera.hpp"

#include "cube.hpp"
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

  void addVoxel(Cube cube);
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

  RenderType render_type_;

  std::vector<Cube> voxel_buffer_;

  static GLFWwindow* window;
  Shader* cube_shader = NULL;

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
};

} // namespace Anthrax
#endif // ANTHRAX_HPP
