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
#include <map>

#define KB(x) ((size_t) (x) << 10)
#define MB(x) ((size_t) (x) << 20)
#define GB(x) ((size_t) (x) << 30)

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

  size_t voxel_cache_size_ = MB(8); // GPU voxel cache size in bytes
  std::vector<Cube> voxel_buffer_;
  size_t voxel_object_size_ = 2*sizeof(glm::vec3) + + 3*sizeof(float) + 2*sizeof(int); // The size (in bytes) of all vertex attributes for a single voxel

  static GLFWwindow* window;
  unsigned int cube_VBO, cube_VAO, cube_EBO;
  unsigned int left_face_vbo_, right_face_vbo_, top_face_vbo_, bottom_face_vbo_, front_face_vbo_, back_face_vbo_;
  unsigned int left_face_vao_, right_face_vao_, top_face_vao_, bottom_face_vao_, front_face_vao_, back_face_vao_;
  unsigned int voxel_vao_, voxels_cache_;
  Shader* cube_shader = NULL;

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

  static constexpr float front_face_vertices_[24] =
  {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f
  };
  static constexpr float back_face_vertices_[24] =
  {
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f
  };
  static constexpr float left_face_vertices_[24] =
  {
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f
  };
  static constexpr float right_face_vertices_[24] =
  {
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f
  };
  static constexpr float bottom_face_vertices_[24] = 
  {
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f
  };
  static constexpr float top_face_vertices_[24] =
  {
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f
  };

};

} // namespace Anthrax
#endif // ANTHRAX_HPP
