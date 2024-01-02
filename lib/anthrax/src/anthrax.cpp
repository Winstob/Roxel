#include "anthrax.hpp"
#include <iostream>

namespace Anthrax
{

// Initialize staic member variables
GLFWwindow* Anthrax::window;
Camera Anthrax::camera;
unsigned int Anthrax::window_width_;
unsigned int Anthrax::window_height_;
unsigned int Anthrax::render_distance_;
float Anthrax::mouse_x_;
float Anthrax::mouse_y_;
float Anthrax::deltaTime;
float Anthrax::lastFrame;
bool Anthrax::wireframe_mode_;


Anthrax::Anthrax()
{
  window = NULL;
  camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));
  window_width_ = 800;
  window_height_ = 600;
  render_distance_ = 10000;
  mouse_x_ = window_width_ / 2.0f;
  mouse_y_ = window_height_ / 2.0f;
  deltaTime = 0.0f;
  lastFrame = 0.0f;
  wireframe_mode_ = false;
}

int Anthrax::startWindow()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  window = glfwCreateWindow(window_width_, window_height_, "Roxel", NULL, NULL);
  //window = glfwCreateWindow(window_width_, window_height_, "Roxel", glfwGetPrimaryMonitor(), NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetKeyCallback(window, key_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile our shader zprogram
  // ------------------------------------
  cube_shader = new Shader(Shader::ShaderInputType::CODESTRING, cube_vertex_shader.c_str(), cube_fragment_shader.c_str(), cube_geometry_shader.c_str());

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // Create vao to render voxels - no data is needed here as everything is computed in the geometry shader
  glGenVertexArrays(1, &voxel_vao_);
  glBindVertexArray(voxel_vao_);

  glGenBuffers(1, &voxels_cache_);
  glBindBuffer(GL_ARRAY_BUFFER, voxels_cache_);
  glBufferData(GL_ARRAY_BUFFER, voxel_cache_size_, NULL, GL_DYNAMIC_DRAW);
  // Fill voxel cache with 0's
  // Probably not the most effective way to do this, but it only happens once
  uint8_t zero = 0;
  for (unsigned int i = 0; i < voxel_cache_size_; i++)
  {
    glBufferSubData(GL_ARRAY_BUFFER, i, 1, &zero);
  }

  // Set up vertex attributes
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribDivisor(0, 1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)(sizeof(glm::vec4)));
  glEnableVertexAttribArray(1);
  glVertexAttribDivisor(1, 1);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)(2*sizeof(glm::vec4)));
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)(3*sizeof(glm::vec4)));
  glEnableVertexAttribArray(3);
  glVertexAttribDivisor(3, 1);

  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)(sizeof(glm::mat4)));
  glEnableVertexAttribArray(4);
  glVertexAttribDivisor(4, 1);
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)(sizeof(glm::mat4) + sizeof(glm::vec3)));
  glEnableVertexAttribArray(5);
  glVertexAttribDivisor(5, 1);
  glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)(sizeof(glm::mat4) + sizeof(glm::vec3) + sizeof(float)));
  glEnableVertexAttribArray(6);
  glVertexAttribDivisor(6, 1);
  glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, voxel_object_size_, (void*)(sizeof(glm::mat4) + sizeof(glm::vec3) + 2*sizeof(float)));
  glEnableVertexAttribArray(7);
  glVertexAttribDivisor(7, 1);
  glVertexAttribPointer(8, 1, GL_INT, GL_FALSE, voxel_object_size_, (void*)(sizeof(glm::mat4) + sizeof(glm::vec3) + 3*sizeof(float)));
  glEnableVertexAttribArray(8);
  glVertexAttribDivisor(8, 1);




  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
 
  /*
  // I couldn't figure out a way to use normal vectors with this method, but keeping it here in case I do in the future
  float cube_vertices_[24] = {
    // Front face
    -0.5f, -0.5f, -0.5f, // Bottom left
    0.5f, -0.5f, -0.5f, // Bottom right
    -0.5f, 0.5f, -0.5f, // Top left
    0.5f, 0.5f, -0.5f, // Top right
    // Back face
    -0.5f, -0.5f, 0.5f, // Bottom left
    0.5f, -0.5f, 0.5f, // Bottom right
    -0.5f, 0.5f, 0.5f, // Top left
    0.5f, 0.5f, 0.5f // Top right
  };
  unsigned int cube_indices_[36] = {
    // Front face
    0, 1, 2,
    1, 2, 3,
    // Back face
    4, 5, 6,
    5, 6, 7,
    // Left face
    0, 4, 6,
    0, 2, 6,
    // Right face
    1, 3, 5,
    3, 5, 7,
    // Top face
    2, 3, 7,
    2, 6, 7,
    // Bottom face
    0, 1, 4,
    1, 4, 5
  };
  glGenVertexArrays(1, &cube_VAO);
  glGenBuffers(1, &cube_VBO);
  glGenBuffers(1, &cube_EBO);

  glBindVertexArray(cube_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices_), cube_vertices_, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices_), cube_indices_, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  */

  // Wireframe mode
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
  // Face culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  return 1;
}


int Anthrax::renderFrame()
{
  // render loop
  // -----------
  // per-frame time logic
  // --------------------
  float currentFrame = static_cast<float>(glfwGetTime());
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  // input
  // -----
  processInput(window);

  // render
  // ------
  //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

  renderScene();

  // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
  // -------------------------------------------------------------------------------
  glfwSwapBuffers(window);
  glfwPollEvents();

  if (glfwWindowShouldClose(window))
  {
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &voxel_vao_);
    glDeleteBuffers(1, &voxels_cache_);
    //glDeleteBuffers(1, &cube_EBO);
    delete(cube_shader);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 1;
  }
  return 0;
}


void Anthrax::renderScene()
{
  glBindVertexArray(voxel_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, voxels_cache_);
  // Set up shader
  cube_shader->use();

  // Camera/view settings
  // Set camera position
  cube_shader->setVec3("view_position", camera.position_);
  // Camera/view transformation
  glm::mat4 view = camera.GetViewMatrix();
  cube_shader->setMat4("view", view);
  // Pass projection matrix to shader (note that in this case it could change every frame)
  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window_width_ / (float)window_height_, 0.1f, (float)render_distance_);
  cube_shader->setMat4("projection", projection);

  // Set sunlight settings
  //cube_shader->setVec3("sunlight.direction", glm::vec3(-1.0f, -0.5f, 0.0f));
  cube_shader->setVec3("sunlight.direction", glm::vec3(glm::cos(glfwGetTime()/16), glm::sin(glfwGetTime()/16), 0.0f));
  cube_shader->setVec3("sunlight.ambient", glm::vec3(0.8));
  cube_shader->setVec3("sunlight.diffuse", glm::vec3(1.0));
  cube_shader->setVec3("sunlight.specular", glm::vec3(0.3));


  for (unsigned int i = 0; i < voxel_buffer_.size(); i++)
  {
    Cube current_cube = voxel_buffer_[i];

    // Get cube material settings
    glm::vec3 color = current_cube.getColor();
    float reflectivity = current_cube.getReflectivity();
    float shininess = current_cube.getShininess();
    float opacity = current_cube.getOpacity();

    // set cube position and scale
    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    model = glm::translate(model, current_cube.getPosition());
    model = glm::scale(model, glm::vec3(current_cube.getSize()));

    int render_faces = 0;
    if (current_cube.render_face_[0]) render_faces |= 1;
    if (current_cube.render_face_[1]) render_faces |= 2;
    if (current_cube.render_face_[2]) render_faces |= 4;
    if (current_cube.render_face_[3]) render_faces |= 8;
    if (current_cube.render_face_[4]) render_faces |= 16;
    if (current_cube.render_face_[5]) render_faces |= 32;

    // Add this cube to the VBO
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_, sizeof(glm::mat4), &model);
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_ + sizeof(glm::mat4), sizeof(glm::vec3), &color);
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_ + sizeof(glm::mat4) + sizeof(glm::vec3), sizeof(float), &reflectivity);
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_ + sizeof(glm::mat4) + sizeof(glm::vec3) + sizeof(float), sizeof(float), &shininess);
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_ + sizeof(glm::mat4) + sizeof(glm::vec3) + 2*sizeof(float), sizeof(float), &opacity);
    glBufferSubData(GL_ARRAY_BUFFER, i*voxel_object_size_ + sizeof(glm::mat4) + sizeof(glm::vec3) + 3*sizeof(float), sizeof(int), &render_faces);
  }


  glDrawArraysInstanced(GL_POINTS, 0, 1, voxel_cache_size_ / voxel_object_size_);
  //glDrawArrays(GL_TRIANGLES, 0, 6);
  //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

  voxel_buffer_.clear();
  glBindVertexArray(0);

}

void Anthrax::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_0 && action  == GLFW_PRESS)
  {
    if (wireframe_mode_)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      wireframe_mode_ = false;
    }
    else
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      wireframe_mode_ = true;
    }
  }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Anthrax::processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Anthrax::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
  window_width_ = width;
  window_height_ = height;
  mouse_x_ = window_width_ / 2.0f;
  mouse_y_ = window_height_ / 2.0f;
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Anthrax::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  mouse_x_ = xpos;
  mouse_y_ = ypos;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Anthrax::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

bool Anthrax::getKeyPress(Key key)
{
  switch(key)
  {
    case Key::A:
      return (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
    case Key::D:
      return (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
    case Key::S:
      return (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
    case Key::W:
      return (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
    case Key::SPACE:
      return (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    case Key::LSHIFT:
      return (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    case Key::LCTRL:
      return (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
    default:
      return false;
  }
}

void Anthrax::getMousePosition(MousePosition *mouse_pos)
{
  mouse_pos->setX(mouse_x_);
  mouse_pos->setY(mouse_y_);
}

void Anthrax::addVoxel(Cube cube)
{
  voxel_buffer_.push_back(cube);
}

void Anthrax::setCameraPosition(vec3<float> position)
{
  camera.setPosition(glm::vec3(position.getX(), position.getY(), position.getZ()));
}

void Anthrax::setCameraRotation(Quaternion rotation)
{
  camera.setRotation(glm::quat(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ()));
}

} // namespace Anthrax
