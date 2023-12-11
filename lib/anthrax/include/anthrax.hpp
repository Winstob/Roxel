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

#include "anthrax_types.hpp"
#include <vector>

namespace Anthrax
{

class Anthrax
{
public:
  Anthrax();
  static bool getKeyPress(Key key);

  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
  static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
  static void processInput(GLFWwindow *window);

  int startWindow();
  int renderFrame();

  void addVoxel(Cube cube);


private:
  std::vector<Cube> voxel_buffer_;

  static GLFWwindow* window;
  unsigned int cube_VBO, cube_VAO, cube_EBO;
  Shader* cube_shader = NULL;

  // settings
  static unsigned int SCR_WIDTH;
  static unsigned int SCR_HEIGHT;
  static int SPEED;
  static unsigned int RENDER_DISTANCE;

  // camera
  static Camera camera;
  static float lastX;
  static float lastY;
  static bool firstMouse;

  // timing
  static float deltaTime;	// time between current frame and last frame
  static float lastFrame;

};

} // namespace Anthrax
#endif // ANTHRAX_HPP
