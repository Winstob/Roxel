#include <string>

const std::string cube_vertex_shader = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 vertex_normal;
out vec3 fragment_position;
out vec3 view_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 view_position;
  
void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f) * vec4(-1.0f, 1.0f, 1.0f, 1.0f);
  fragment_position = vec3(model * vec4(position, 1.0));
  vertex_normal = normalize(normal);
  view_pos = view_position;
}
)glsl";


const std::string cube_fragment_shader = R"glsl(
#version 330 core

in float vertex_opacity;
in vec3 vertex_normal;
in vec3 fragment_position;
in vec3 view_pos;

struct DirectLight
{
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Material
{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

uniform DirectLight sunlight;
uniform Material material;
uniform float opacity;

void main()
{
  // Calculate sunlight phong vector
  // Calculate ambient light
  vec3 sunlight_ambient = sunlight.ambient * material.ambient;
  // Calculate diffuse light
  vec3 normal = normalize(vertex_normal);
  float diff = max(dot(normal, -normalize(sunlight.direction)), 0.0);
  vec3 sunlight_diffuse = sunlight.diffuse * (diff * material.diffuse);

  // Calculate specular light
  vec3 view_direction = normalize(view_pos - fragment_position);
  vec3 reflect_direction = reflect(normalize(sunlight.direction), normal);
  vec3 halfway_direction = normalize(normalize(-sunlight.direction) + view_direction);
  //float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess * 128);
  float spec = pow(max(dot(vertex_normal, halfway_direction), 0.0), material.shininess * 256);

  vec3 sunlight_specular = sunlight.specular * (spec * material.specular);

  // Calculate Phong model lighting
  vec3 phong = sunlight_ambient + sunlight_diffuse + sunlight_specular;

  gl_FragColor = vec4(phong, vertex_opacity);
}
)glsl";
