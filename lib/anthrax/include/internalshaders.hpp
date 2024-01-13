/* ---------------------------------------------------------------- *\
 * internalshaders.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-11-30
\* ---------------------------------------------------------------- */


#include <string>

const std::string cube_vertex_shader = R"glsl(
#version 330 core
#extension GL_EXT_gpu_shader4 : enable

layout (location = 0) in vec3 voxel_position;
layout (location = 1) in int voxel_size;
layout (location = 2) in uint render_faces;

// Material settings
layout (location = 3) in vec3 color;
layout (location = 4) in float reflectivity;
layout (location = 5) in float shininess;
layout (location = 6) in float opacity;

out mat4 model;

flat out float render_left;
flat out float render_right;
flat out float render_bottom;
flat out float render_top;
flat out float render_front;
flat out float render_back;

// Material settings
flat out vec3 vertex_color;
flat out float vertex_reflectivity;
flat out float vertex_shininess;
flat out float vertex_opacity;


void main()
{
	//gl_Position = projection * view * model * vec4(0.0, 0.0, 0.0, 1.0f) * vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
  mat4 pos_matrix = mat4(vec4(1.0, 0.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0), vec4(0.0, 0.0, 1.0, 0.0), vec4(voxel_position, 1.0));
  mat4 scale_matrix = mat4(vec4(intBitsToFloat(voxel_size), 0.0, 0.0, 0.0), vec4(0.0, intBitsToFloat(voxel_size), 0.0, 0.0), vec4(0.0, 0.0, intBitsToFloat(voxel_size), 0.0), vec4(0.0, 0.0, 0.0, 1.0));
  model = pos_matrix * scale_matrix * mat4(1.0);

  // Material settings
  //vertex_color = vec3(1.0, 1.0, 1.0);
  vertex_color = color;
  vertex_reflectivity = reflectivity;
  vertex_shininess = shininess;
  vertex_opacity = opacity;

  // For whatever reason, opengl always interprets data as floats, even when it is specified as an int
  // So we have to do some finagling to parse this int properly
  float faces_to_render = uintBitsToFloat(render_faces);

  render_back = 0.0;
  if (faces_to_render >= 32.0)
  {
    render_back = 1.0;
    faces_to_render -= 32.0;
  }
  render_front = 0.0;
  if (faces_to_render >= 16.0)
  {
    render_front = 1.0;
    faces_to_render -= 16.0;
  }
  render_top = 0.0;
  if (faces_to_render >= 8.0)
  {
    render_top = 1.0;
    faces_to_render -= 8.0;
  }
  render_bottom = 0.0;
  if (faces_to_render >= 4.0)
  {
    render_bottom = 1.0;
    faces_to_render -= 4.0;
  }
  render_right = 0.0;
  if (faces_to_render >= 2.0)
  {
    render_right = 1.0;
    faces_to_render -= 2.0;
  }
  render_left = 0.0;
  if (faces_to_render >= 1.0)
  {
    render_left = 1.0;
  }

}
)glsl";

const std::string cube_fragment_shader1 = R"glsl(
#version 330 core

in vec3 normal;
in vec3 fragment_position;

// Material settings
in vec3 voxel_color;
in float voxel_reflectivity;
in float voxel_shininess;
in float voxel_opacity;

struct DirectLight
{
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform DirectLight sunlight;
uniform vec3 view_position;


void main()
{
  gl_FragColor = vec4(voxel_color, voxel_opacity);
}
)glsl";

const std::string cube_fragment_shader = R"glsl(
#version 330 core

in vec3 normal;
in vec3 fragment_position;

// Material settings
flat in vec3 voxel_color;
flat in float voxel_reflectivity;
flat in float voxel_shininess;
flat in float voxel_opacity;

struct DirectLight
{
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform DirectLight sunlight;
uniform vec3 view_position;

void main()
{
  // Calculate sunlight phong vector
  // Calculate ambient light
  vec3 sunlight_ambient = sunlight.ambient * voxel_color;
  // Calculate diffuse light
  float diff = max(dot(normal, -normalize(sunlight.direction)), 0.0);
  vec3 sunlight_diffuse = sunlight.diffuse * (diff * voxel_color);

  // Calculate specular light
  vec3 view_direction = normalize(view_position - fragment_position);
  vec3 reflect_direction = reflect(normalize(sunlight.direction), normal);
  vec3 halfway_direction = normalize(normalize(-sunlight.direction) + view_direction);
  //float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess * 128);
  float spec = pow(max(dot(normal, halfway_direction), 0.0), voxel_shininess * 256);

  vec3 sunlight_specular = sunlight.specular * (spec * (voxel_reflectivity * voxel_color));

  // Calculate Phong model lighting
  vec3 phong = sunlight_ambient + sunlight_diffuse + sunlight_specular;

  gl_FragColor = vec4(phong, voxel_opacity);
}
)glsl";


const std::string cube_geometry_shader = R"glsl(
#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;
//layout (triangle_strip, max_vertices = 3) out;

in mat4 model[];

flat in float render_left[];
flat in float render_right[];
flat in float render_bottom[];
flat in float render_top[];
flat in float render_front[];
flat in float render_back[];

out vec3 normal;
out vec3 fragment_position;


// Material settings (transparent)
flat in vec3 vertex_color[];
flat in float vertex_reflectivity[];
flat in float vertex_shininess[];
flat in float vertex_opacity[];
flat out vec3 voxel_color;
flat out float voxel_reflectivity;
flat out float voxel_shininess;
flat out float voxel_opacity;


uniform mat4 view;
uniform mat4 projection;

void drawFrontFace(vec4 position)
{
  normal = vec3(0.0, 0.0, -1.0);
  fragment_position = vec3(model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawBackFace(vec4 position)
{
  normal = vec3(0.0, 0.0, 1.0);
  fragment_position = vec3(model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawLeftFace(vec4 position)
{
  normal = vec3(-1.0, 0.0, 0.0);
  fragment_position = vec3(model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawRightFace(vec4 position)
{
  normal = vec3(1.0, 0.0, 0.0);
  fragment_position = vec3(model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawBottomFace(vec4 position)
{
  normal = vec3(0.0, -1.0, 0.0);
  fragment_position = vec3(model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawTopFace(vec4 position)
{
  normal = vec3(0.0, 1.0, 0.0);
  fragment_position = vec3(model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

void main()
{
  // Transparents
  voxel_color = vertex_color[0];
  voxel_reflectivity = vertex_reflectivity[0];
  voxel_shininess = vertex_shininess[0];
  voxel_opacity = vertex_opacity[0];

  if (render_left[0] == 1.0) drawLeftFace(vec4(0.0, 0.0, 0.0, 1.0));
  if (render_right[0] == 1.0) drawRightFace(vec4(0.0, 0.0, 0.0, 1.0));
  if (render_bottom[0] == 1.0) drawBottomFace(vec4(0.0, 0.0, 0.0, 1.0));
  if (render_top[0] == 1.0) drawTopFace(vec4(0.0, 0.0, 0.0, 1.0));
  if (render_front[0] == 1.0) drawFrontFace(vec4(0.0, 0.0, 0.0, 1.0));
  if (render_back[0] == 1.0) drawBackFace(vec4(0.0, 0.0, 0.0, 1.0));
}
)glsl";


const std::string screen_vertex_shader = R"glsl(
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texture_coordinates;

out vec2 TexCoords;

void main()
{
  gl_Position = vec4(position.x, position.y, 0.0, 1.0); 
  TexCoords = texture_coordinates;
}
)glsl";


const std::string screen_fragment_shader = R"glsl(
#version 330 core
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
  gl_FragColor = texture(screenTexture, TexCoords);
}
)glsl";
