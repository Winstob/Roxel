/* ---------------------------------------------------------------- *\
 * internalshaders.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-11-30
\* ---------------------------------------------------------------- */


// NOTE: within all shaders, a left-handed coordinate system is used.


#include <string>

const std::string geometry_pass_vshader = R"glsl(
#version 330 core
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


const std::string geometry_pass_fshader = R"glsl(
#version 330 core

layout (location = 0) out vec3 g_position_texture_;
layout (location = 1) out vec3 g_normal_texture_;
layout (location = 2) out vec3 g_color_texture_;
layout (location = 3) out vec3 g_material_texture_;

in vec3 normal;
in vec3 fragment_position;

// Material settings
flat in vec3 voxel_color;
flat in float voxel_reflectivity;
flat in float voxel_shininess;
flat in float voxel_opacity;

void main()
{
  g_position_texture_ = fragment_position;
  g_normal_texture_ = normalize(normal);
  g_color_texture_ = voxel_color;
  g_material_texture_ = vec3(voxel_reflectivity, voxel_shininess, voxel_opacity);
}


)glsl";


const std::string geometry_pass_gshader = R"glsl(
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

void drawFrontFace()
{
  normal = vec3(0.0, 0.0, 1.0);
  fragment_position = (model[0] * vec4(-0.5, -0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * (vec4(-0.5, -0.5, 0.5, 1.0));
  EmitVertex();
  fragment_position = (model[0] * vec4(0.5, -0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(0.5, -0.5, 0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(-0.5, 0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(-0.5, 0.5, 0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(0.5, 0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(0.5, 0.5, 0.5, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawBackFace()
{
  normal = vec3(0.0, 0.0, -1.0);
  fragment_position = (model[0] * vec4(0.5, 0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * (vec4(0.5, 0.5, -0.5, 1.0));
  EmitVertex();
  fragment_position = (model[0] * vec4(0.5, -0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(0.5, -0.5, -0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(-0.5, 0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(-0.5, 0.5, -0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(-0.5, -0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(-0.5, -0.5, -0.5, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawLeftFace()
{
  normal = vec3(-1.0, 0.0, 0.0);
  fragment_position = (model[0] * vec4(-0.5, -0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * (vec4(-0.5, -0.5, 0.5, 1.0));
  EmitVertex();
  fragment_position = (model[0] * vec4(-0.5, 0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(-0.5, 0.5, 0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(-0.5, -0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(-0.5, -0.5, -0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(-0.5, 0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(-0.5, 0.5, -0.5, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawRightFace()
{
  normal = vec3(1.0, 0.0, 0.0);
  fragment_position = (model[0] * vec4(0.5, 0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * (vec4(0.5, 0.5, -0.5, 1.0));
  EmitVertex();
  fragment_position = (model[0] * vec4(0.5, 0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(0.5, 0.5, 0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(0.5, -0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(0.5, -0.5, -0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(0.5, -0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(0.5, -0.5, 0.5, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawBottomFace()
{
  normal = vec3(0.0, -1.0, 0.0);
  fragment_position = (model[0] * vec4(0.5, -0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * (vec4(0.5, -0.5, -0.5, 1.0));
  EmitVertex();
  fragment_position = (model[0] * vec4(0.5, -0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(0.5, -0.5, 0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(-0.5, -0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(-0.5, -0.5, -0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(-0.5, -0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(-0.5, -0.5, 0.5, 1.0);
  EmitVertex();
  EndPrimitive();
}

void drawTopFace()
{
  normal = vec3(0.0, 1.0, 0.0);
  fragment_position = (model[0] * vec4(-0.5, 0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * (vec4(-0.5, 0.5, 0.5, 1.0));
  EmitVertex();
  fragment_position = (model[0] * vec4(0.5, 0.5, 0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(0.5, 0.5, 0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(-0.5, 0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(-0.5, 0.5, -0.5, 1.0);
  EmitVertex();
  fragment_position = (model[0] * vec4(0.5, 0.5, -0.5, 1.0)).xyz;
  gl_Position = projection * view * model[0] * vec4(0.5, 0.5, -0.5, 1.0);
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

  if (render_left[0] == 1.0) drawLeftFace();
  if (render_right[0] == 1.0) drawRightFace();
  if (render_bottom[0] == 1.0) drawBottomFace();
  if (render_top[0] == 1.0) drawTopFace();
  if (render_front[0] == 1.0) drawFrontFace();
  if (render_back[0] == 1.0) drawBackFace();
}
)glsl";


const std::string ssao_pass_vshader = R"glsl(
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texture_coordinates;

out vec2 tex_coords;

void main()
{
  tex_coords = texture_coordinates;
  gl_Position = vec4(position, 0.0, 1.0);
}
)glsl";


const std::string ssao_pass_fshader = R"glsl(
#version 330 core
layout (location = 0) out float occlusion_factor;

uniform sampler2D g_position_texture_;
uniform sampler2D g_normal_texture_;
uniform sampler2D ssao_noise_texture_;

uniform vec3 samples[128];

uniform mat4 view;
uniform mat4 projection;
uniform float do_ambient_occlusion;

in vec2 tex_coords;

const float kernel_size = 128.0;
const float radius = 5.0;
const float bias = 0.025;



void main()
{
  if (do_ambient_occlusion == 0.0)
  {
    occlusion_factor = 1.0;
    return;
  }

  // get input for SSAO algorithm
  vec3 frag_pos = (view * vec4(texture(g_position_texture_, tex_coords).xyz, 1.0)).xyz;
  vec3 frag_world_pos = texture(g_position_texture_, tex_coords).xyz;
  //vec3 normal = normalize((transpose(inverse(view)) * vec4(texture(g_normal_texture_, tex_coords).rgb, 1.0)).xyz);
  vec3 normal = normalize(texture(g_normal_texture_, tex_coords).rgb);
  vec3 random_vec = normalize(texture(ssao_noise_texture_, tex_coords).xyz);

  // create TBN change-of-basis matrix: from tangent-space to view-space
  // Two wys to create this: TBN matrix or a slightly faster method that only works for axis-aligned normals
  // ----- TBN ----- \\
  // NOTE: With this method, the samples must be within a hemisphere with +z normal
  /*
  vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN = mat3(tangent, bitangent, normal);
  */
  // ----- AXIS-ALIGNED ----- \\
  // NOTE: With this method, the samples must be within a hemisphere with +y normal
  mat3 TBN = mat3(
      normal.y + abs(normal.z), -normal.x, 0,
      normal.x, normal.y, normal.z,
      0, -normal.z, normal.y + abs(normal.x)
  );

  // iterate over the sample kernel and calculate occlusion factor
  float occlusion = 0.0;
  for(int i = 0; i < kernel_size; ++i)
  {
    // get sample position
    vec3 sample_pos = TBN * samples[i]; // from tangent to view-space
    sample_pos = frag_world_pos + sample_pos * radius; 
    sample_pos = (view * vec4(sample_pos, 1.0)).xyz;
    
    // project sample position (to sample texture) (to get position on screen/texture)
    vec4 offset = vec4(sample_pos, 1.0);
    offset = projection * offset; // from view to clip-space
    offset.xyz /= offset.w; // perspective divide
    offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
    
    // get sample depth
    float sample_depth = (view * vec4(texture(g_position_texture_, offset.xy).xyz, 1.0)).z; // get depth value of kernel sample
    
    // range check & accumulate
    float range_check = smoothstep(0.0, 1.0, radius / abs(frag_pos.z - sample_depth));
    occlusion += (sample_depth >= sample_pos.z + bias ? 1.0 : 0.0) * range_check;
  }
  occlusion = 1.0 - (occlusion / kernel_size);
  
  occlusion_factor = occlusion;
}

)glsl";


const std::string ssao_blur_pass_vshader = R"glsl(
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texture_coordinates;

out vec2 tex_coords;

void main()
{
    tex_coords = texture_coordinates;
    gl_Position = vec4(position, 0.0, 1.0);
}
)glsl";


const std::string ssao_blur_pass_fshader = R"glsl(
#version 330 core
layout (location = 0) out float frag_color;

uniform sampler2D ssao_texture_;
uniform float blur_radius;

in vec2 tex_coords;

void main()
{
  vec2 texel_size = 1.0 / vec2(textureSize(ssao_texture_, 0));
  float result = 0.0;
  for (float x = -blur_radius; x < blur_radius; ++x)
  {
    for (float y = -blur_radius; y < blur_radius; ++y)
    {
      vec2 offset = vec2(x, y) * texel_size;
      result += texture(ssao_texture_, tex_coords + offset).r;
    }
  }
  frag_color = result / (4.0 * blur_radius * blur_radius);
}
)glsl";


const std::string lighting_pass_vshader = R"glsl(
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texture_coordinates;

out vec2 tex_coords;

void main()
{
    tex_coords = texture_coordinates;
    gl_Position = vec4(position, 0.0, 1.0);
}
)glsl";


const std::string lighting_pass_fshader = R"glsl(
#version 330 core

in vec2 tex_coords;

struct DirectLight
{
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform DirectLight sunlight;
uniform vec3 view_position;

uniform sampler2D g_position_texture_;
uniform sampler2D g_normal_texture_;
uniform sampler2D g_color_texture_;
uniform sampler2D g_material_texture_;
uniform sampler2D ssao_texture_;

void main()
{             
  // retrieve data from gbuffer
  vec3 in_world_position = texture(g_position_texture_, tex_coords).rgb;
  vec3 normal = texture(g_normal_texture_, tex_coords).rgb;
  vec3 voxel_color = texture(g_color_texture_, tex_coords).rgb;
  float voxel_reflectivity = texture(g_material_texture_, tex_coords).r;
  float voxel_shininess = texture(g_material_texture_, tex_coords).g;
  float voxel_opacity = texture(g_material_texture_, tex_coords).b;
  float ambient_occlusion = texture(ssao_texture_, tex_coords).r;
  
  // Calculate sunlight phong vector
  // Calculate ambient light
  vec3 sunlight_ambient = sunlight.ambient * voxel_color * ambient_occlusion;
  // Calculate diffuse light
  float diff = max(dot(normal, -normalize(sunlight.direction)), 0.0);
  vec3 sunlight_diffuse = sunlight.diffuse * (diff * voxel_color);

  // Calculate specular light
  vec3 view_direction = normalize(view_position - in_world_position);
  vec3 reflect_direction = reflect(normalize(sunlight.direction), normal);
  vec3 halfway_direction = normalize(normalize(-sunlight.direction) + view_direction);
  float spec = pow(max(dot(normal, halfway_direction), 0.0), voxel_shininess * 256);

  vec3 sunlight_specular = sunlight.specular * (spec * (voxel_reflectivity * voxel_color));

  // Calculate Phong model lighting
  vec3 phong = sunlight_ambient + sunlight_diffuse + sunlight_specular;

  gl_FragColor = vec4(phong, voxel_opacity);
}
)glsl";
