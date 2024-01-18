/* ---------------------------------------------------------------- *\
 * anthrax.cpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-10
\* ---------------------------------------------------------------- */

#include <iostream>
#include <random>
#include "anthrax.hpp"

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
bool Anthrax::window_size_changed_ = true;


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

  // build and compile our shader programs
  // -------------------------------------
  geometry_pass_shader_ = new Shader(Shader::ShaderInputType::CODESTRING, geometry_pass_vshader.c_str(), geometry_pass_fshader.c_str(), geometry_pass_gshader.c_str());

  ssao_pass_shader_ = new Shader(Shader::ShaderInputType::CODESTRING, ssao_pass_vshader.c_str(), ssao_pass_fshader.c_str());

  lighting_pass_shader_ = new Shader(Shader::ShaderInputType::CODESTRING, lighting_pass_vshader.c_str(), lighting_pass_fshader.c_str());

  ssao_pass_shader_->use();
  ssao_pass_shader_->setInt("g_position_texture_", 0);
  ssao_pass_shader_->setInt("g_normal_texture_", 1);
  ssao_pass_shader_->setInt("ssao_noise_texture_", 2);
  lighting_pass_shader_->use();
  lighting_pass_shader_->setInt("g_position_texture_", 0);
  lighting_pass_shader_->setInt("g_normal_texture_", 1);
  lighting_pass_shader_->setInt("g_color_texture_", 2);
  lighting_pass_shader_->setInt("g_material_texture_", 3);
  lighting_pass_shader_->setInt("ssao_texture_", 4);
  /*
  geometry_pass_shader_->setInt("g_position_texture_", 0);
  geometry_pass_shader_->setInt("g_normal_texture_", 1);
  geometry_pass_shader_->setInt("g_albedo_texture_", 2);
  */

  cube_shader_ = new Shader(Shader::ShaderInputType::CODESTRING, cube_vertex_shader.c_str(), cube_fragment_shader.c_str(), cube_geometry_shader.c_str());
  screen_shader_ = new Shader(Shader::ShaderInputType::CODESTRING, screen_vertex_shader.c_str(), screen_fragment_shader.c_str());

  gBufferSetup();
  ssaoFramebufferSetup();
  ssaoKernelSetup();

  // Initialize the voxel cache
  voxel_cache_manager_ = new VoxelCacheManager();
  voxel_cache_manager_->initialize(MB(8), [](glm::vec3 position)
      {
      return (glm::length(position - camera.position_) < (256 << 6));// && 2*glm::angle(glm::normalize(position - camera.position_), camera.getLookDirection()) < 3.14/3);
      });

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

  if (window_size_changed_)
  {
    resizeWindow();
    gBufferSetup();
    ssaoFramebufferSetup();
    window_size_changed_ = false;
  }
  if (wireframe_mode_)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  // Update the voxel cache
  // Temporary: give player position to cache manager
  voxel_cache_manager_->view_position_ = camera.position_;
  voxel_cache_manager_->updateCache();

  // render
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_);
  glEnable(GL_DEPTH_TEST);
  //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

  // Render the scene to the g-buffer
  renderScene();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_DEPTH_TEST);

  // SSAO pass
  glBindFramebuffer(GL_FRAMEBUFFER, ssao_framebuffer_);
  glClear(GL_COLOR_BUFFER_BIT);
  ssao_pass_shader_->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_position_texture_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_normal_texture_);
  ssao_pass_shader_->setMat4("view", camera.GetViewMatrix());
  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window_width_ / (float)window_height_, 0.1f, (float)render_distance_);
  ssao_pass_shader_->setMat4("projection", projection);
  renderQuad();

  // Go back the default framebuffer and draw the scene to the screen
  // Lighting pass
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  lighting_pass_shader_->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_position_texture_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_normal_texture_);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, g_color_texture_);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, g_material_texture_);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, ssao_texture_);
  lighting_pass_shader_->setVec3("view_position", camera.position_);
  lighting_pass_shader_->setVec3("sunlight.direction", glm::vec3(glm::cos(glfwGetTime()/16), glm::sin(glfwGetTime()/16), 0.0f));
  lighting_pass_shader_->setVec3("sunlight.ambient", glm::vec3(0.5, 0.5, 0.7));
  lighting_pass_shader_->setVec3("sunlight.diffuse", glm::vec3(0.4, 0.4, 0.2));
  lighting_pass_shader_->setVec3("sunlight.specular", glm::vec3(0.3));
  renderQuad();
  /*
  screen_shader_->use();
  glBindTexture(GL_TEXTURE_2D, texture_color_buffer_);
  renderQuad();
  */

  // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
  // -------------------------------------------------------------------------------
  glfwSwapBuffers(window);
  glfwPollEvents();

  if (glfwWindowShouldClose(window))
  {
    delete ssao_pass_shader_;
    delete geometry_pass_shader_;
    delete lighting_pass_shader_;
    delete cube_shader_;
    delete screen_shader_;

    glDeleteFramebuffers(1, &g_buffer_);
    glDeleteTextures(1, &g_position_texture_);
    glDeleteTextures(1, &g_normal_texture_);
    glDeleteTextures(1, &g_color_texture_);
    glDeleteTextures(1, &g_material_texture_);
    glDeleteRenderbuffers(1, &g_depth_rbo_);

    glDeleteFramebuffers(1, &ssao_framebuffer_);
    glDeleteTextures(1, &ssao_texture_);

    glDeleteVertexArrays(1, &quad_vao_);
    glDeleteBuffers(1, &quad_vbo_);
    glDeleteRenderbuffers(1, &renderbuffer_object_);
    glDeleteFramebuffers(1, &framebuffer_);

    delete voxel_cache_manager_;
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 1;
  }
  return 0;
}


void Anthrax::renderScene()
{
  // Set up shader
  geometry_pass_shader_->use();

  // Camera/view transformation
  glm::mat4 view = camera.GetViewMatrix();
  geometry_pass_shader_->setMat4("view", view);
  // Pass projection matrix to shader (note that in this case it could change every frame)
  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window_width_ / (float)window_height_, 0.1f, (float)render_distance_);
  geometry_pass_shader_->setMat4("projection", projection);

  // Set sunlight settings
  //cube_shader->setVec3("sunlight.direction", glm::vec3(-1.0f, -0.5f, 0.0f));
  /*
  cube_shader_->setVec3("sunlight.direction", glm::vec3(glm::cos(glfwGetTime()/16), glm::sin(glfwGetTime()/16), 0.0f));
  cube_shader_->setVec3("sunlight.ambient", glm::vec3(0.5, 0.5, 0.7));
  cube_shader_->setVec3("sunlight.diffuse", glm::vec3(0.4, 0.4, 0.2));
  cube_shader_->setVec3("sunlight.specular", glm::vec3(0.3));
  */

  if (voxel_buffer_.size() > 0)
  {
    voxel_cache_manager_->addCubes(&(voxel_buffer_[0]), voxel_buffer_.size());
    voxel_buffer_.clear();
  }
  voxel_cache_manager_->renderCubes();
}


void Anthrax::gBufferSetup()
{
  if (g_buffer_ == 0)
  {
    glGenFramebuffers(1, &g_buffer_);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_);

  if (g_position_texture_ == 0)
  {
    glGenTextures(1, &g_position_texture_);
  }
  if (g_normal_texture_ == 0)
  {
    glGenTextures(1, &g_normal_texture_);
  }
  if (g_color_texture_ == 0)
  {
    glGenTextures(1, &g_color_texture_);
  }
  if (g_material_texture_ == 0)
  {
    glGenTextures(1, &g_material_texture_);
  }
  if (g_depth_rbo_ == 0)
  {
    glGenRenderbuffers(1, &g_depth_rbo_);
  }

  if (window_size_changed_)
  {
    // position color buffer
    glBindTexture(GL_TEXTURE_2D, g_position_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width_, window_height_, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position_texture_, 0);
    // normal color buffer
    glBindTexture(GL_TEXTURE_2D, g_normal_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width_, window_height_, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal_texture_, 0);
    // color buffer
    glBindTexture(GL_TEXTURE_2D, g_color_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width_, window_height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_color_texture_, 0);
    // material buffer
    glBindTexture(GL_TEXTURE_2D, g_material_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width_, window_height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, g_material_texture_, 0);

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);
    // create and attach depth buffer (renderbuffer)
    glBindRenderbuffer(GL_RENDERBUFFER, g_depth_rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width_, window_height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_depth_rbo_);

    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return;
}


void Anthrax::ssaoFramebufferSetup()
{
  if (ssao_framebuffer_ == 0)
  {
    glGenFramebuffers(1, &ssao_framebuffer_);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, ssao_framebuffer_);

  if (ssao_texture_ == 0)
  {
    glGenTextures(1, &ssao_texture_);
  }

  // ssao color buffer
  glBindTexture(GL_TEXTURE_2D, ssao_texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, window_width_, window_height_, 0, GL_RED, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_texture_, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "Framebuffer not complete!" << std::endl;

  ssao_pass_shader_->use();
  ssao_pass_shader_->setFloat("window_width_", window_width_);
  ssao_pass_shader_->setFloat("window_height_", window_height_);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return;
}


void Anthrax::ssaoKernelSetup()
{
  glBindFramebuffer(GL_FRAMEBUFFER, ssao_framebuffer_);
  ssao_pass_shader_->use();

  std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
  std::default_random_engine generator;
  for (unsigned int i = 0; i < 64; ++i)
  {
    glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator), randomFloats(generator) * 2.0 - 1.0);
    sample = glm::normalize(sample);
    sample *= randomFloats(generator);
    float scale = float(i) / 64.0f;

    // scale samples so they're more aligned to center of kernel
    //scale = lerp(0.1f, 1.0f, scale * scale);
    scale = 1.0f + scale * scale * (1.0f - 0.1f);
    sample *= scale;
    ssao_pass_shader_->setVec3("samples[" + std::to_string(i) + "]", sample);
  }


  std::vector<glm::vec3> ssaoNoise;
  for (unsigned int i = 0; i < 16; i++)
  {
    glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
    ssaoNoise.push_back(noise);
  }

  glGenTextures(1, &ssao_noise_texture_);
  glBindTexture(GL_TEXTURE_2D, ssao_noise_texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  return;
}


void Anthrax::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_0 && action  == GLFW_PRESS)
  {
    if (wireframe_mode_)
    {
      wireframe_mode_ = false;
    }
    else
    {
      wireframe_mode_ = true;
    }
  }
}


void Anthrax::renderQuad()
{
  if (quad_vao_ == 0)
  {
    // Set up fullscreen quad
    float quad_vertices_[] = {
    // positions  // texture coordinates
    -1.0, -1.0,   0.0, 0.0,
    1.0, -1.0,    1.0, 0.0,
    -1.0, 1.0,    0.0, 1.0,
    1.0, 1.0,     1.0, 1.0
    };
    glGenVertexArrays(1, &quad_vao_);
    glGenBuffers(1, &quad_vbo_);
    glBindVertexArray(quad_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices_), &quad_vertices_, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
  }
  glBindVertexArray(quad_vao_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
  return;
}


void Anthrax::resizeWindow()
{
  /*
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
  glBindTexture(GL_TEXTURE_2D, texture_color_buffer_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width_, window_height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer_, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_object_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width_, window_height_); // use a single renderbuffer object for both a depth AND stencil buffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_object_); // now actually attach it
                                                                                                        */

  return;
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
  window_size_changed_ = true;
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

void Anthrax::addVoxel(std::weak_ptr<Cube> cube)
{
  voxel_cache_manager_->addCube(cube);
  //voxel_display_list_.push_back(cube);
  //voxel_buffer_.push_back(*cube);
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
