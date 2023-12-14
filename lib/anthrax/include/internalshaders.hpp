#include <string>

const std::string cube_vertex_shader = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec4 vertex_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f) * vec4(-1.0f, 1.0f, 1.0f, 1.0f);
  vertex_color = color;
}
)glsl";


const std::string cube_fragment_shader = R"glsl(
#version 330 core

in vec4 vertex_color;
out vec4 FragColor;
  
void main()
{
  //FragColor = vec4(0.5, 0.0, 0.0, 1.0);
  FragColor = vertex_color;
}
)glsl";
