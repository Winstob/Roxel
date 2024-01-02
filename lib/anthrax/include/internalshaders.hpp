#include <string>

const std::string cube_vertex_shader = R"glsl(
#version 330 core
layout (location = 0) in mat4 model;

// Material settings
layout (location = 4) in vec3 color;
layout (location = 5) in float reflectivity;
layout (location = 6) in float shininess;
layout (location = 7) in float opacity;

layout (location = 8) in int render_faces;

out mat4 out_model;

out vec3 vertex_position;

// Material settings
out vec3 vertex_color;
out float vertex_reflectivity;
out float vertex_shininess;
out float vertex_opacity;

out int out_render_faces;


void main()
{
	//gl_Position = projection * view * model * vec4(0.0, 0.0, 0.0, 1.0f) * vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
  out_model = model;
  vertex_position = vec3(model * vec4(0.0, 0.0, 0.0, 1.0));

  // Material settings
  vertex_color = vec3(1.0, 1.0, 1.0);
  //voxel_color = color;
  vertex_reflectivity = reflectivity;
  vertex_shininess = shininess;
  vertex_opacity = opacity;

  out_render_faces = render_faces;
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

in mat4 out_model[];
in int out_render_faces[];

out vec3 normal;
out vec3 fragment_position;


// Material settings (transparent)
in vec3 vertex_color[];
in float vertex_reflectivity[];
in float vertex_shininess[];
in float vertex_opacity[];
out vec3 voxel_color;
out float voxel_reflectivity;
out float voxel_shininess;
out float voxel_opacity;


uniform mat4 view;
uniform mat4 projection;

void drawFrontFace(vec4 position)
{
  normal = vec3(0.0, 0.0, -1.0);
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawBackFace(vec4 position)
{
  normal = vec3(0.0, 0.0, 1.0);
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawLeftFace(vec4 position)
{
  normal = vec3(-1.0, 0.0, 0.0);
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawRightFace(vec4 position)
{
  normal = vec3(1.0, 0.0, 0.0);
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawBottomFace(vec4 position)
{
  normal = vec3(0.0, -1.0, 0.0);
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, -0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, -0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawTopFace(vec4 position)
{
  normal = vec3(0.0, 1.0, 0.0);
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, 0.5, -0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(-0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
  EmitVertex();
  fragment_position = vec3(out_model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)));
  gl_Position = projection * view * out_model[0] * (position + vec4(0.5, 0.5, 0.5, 0.0)) * vec4(-1.0, 1.0, 1.0, 1.0);
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

  drawFrontFace(vec4(0.0, 0.0, 0.0, 1.0));
  drawBackFace(vec4(0.0, 0.0, 0.0, 1.0));
  drawLeftFace(vec4(0.0, 0.0, 0.0, 1.0));
  drawRightFace(vec4(0.0, 0.0, 0.0, 1.0));
  drawBottomFace(vec4(0.0, 0.0, 0.0, 1.0));
  drawTopFace(vec4(0.0, 0.0, 0.0, 1.0));
}
)glsl";
