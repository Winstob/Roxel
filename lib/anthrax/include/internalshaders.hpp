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


const std::string ssao_pass_fshader1 = R"glsl(
#version 330 core
layout (location = 0) out float occlusion_factor;

uniform sampler2D g_position_texture_;
uniform sampler2D g_normal_texture_;

uniform vec3 samples[64];

uniform mat4 view;
uniform mat4 projection;

in vec2 tex_coords;

const float radius = 2.5;
const float bias = 0.025;

void main()
{
  vec3 frag_world_pos = texture(g_position_texture_, tex_coords).rgb;
  vec3 frag_view_pos = (view * vec4(frag_world_pos, 1.0)).xyz;
  vec3 frag_normal = texture(g_normal_texture_, tex_coords).rgb;

  float occlusion = 0.0;
  for (int i = 0; i < 64; i++)
  {
    //mat3 normal_transform_matrix = mat3(1.0);
    /*
    mat3 normal_transform_matrix = mat3(
        frag_normal.y + abs(frag_normal.z), frag_normal.x, 0,
        -frag_normal.x, frag_normal.y, -frag_normal.z,
        0, frag_normal.z, frag_normal.y + abs(frag_normal.x)
    );
    */
    mat3 normal_transform_matrix = mat3(
        frag_normal.y + abs(frag_normal.z), -frag_normal.x, 0,
        frag_normal.x, frag_normal.y, frag_normal.z,
        0, -frag_normal.z, frag_normal.y + abs(frag_normal.x)
    );
    vec3 sample_world_pos = frag_world_pos + normal_transform_matrix * samples[i] * radius;
    vec3 sample_view_pos = (view * vec4(sample_world_pos, 1.0)).xyz;
    vec4 tmp = projection * vec4(sample_view_pos, 1.0);
    vec3 sample_screen_pos = tmp.xyz / tmp.w * 0.5 + 0.5;

    tmp = (projection * view * vec4(texture(g_position_texture_, sample_screen_pos.xy).rgb, 1.0));
    float sample_depth_comparison = tmp.z / tmp.w;
    //float sample_depth_comparison = (view * vec4(texture(g_position_texture_, sample_screen_pos.xy).rgb, 1.0)).z; // sus

    occlusion += (sample_depth_comparison >= sample_screen_pos.z + bias ? 1.0 : 0.0);
  }

  occlusion_factor = 1.0 - (occlusion / 64.0);
}
)glsl";


const std::string ssao_pass_fshader = R"glsl(
#version 330 core
layout (location = 0) out float occlusion_factor;

uniform sampler2D g_position_texture_;
uniform sampler2D g_normal_texture_;
uniform sampler2D ssao_noise_texture_;

uniform vec3 samples[64];

uniform mat4 view;
uniform mat4 projection;
uniform float window_width_;
uniform float window_height_;

in vec2 tex_coords;

const float kernelSize = 64.0;
const float radius = 0.5;
const float bias = 0.025;



void main()
{
    vec2 noiseScale = vec2(window_width_/4.0, window_height_/4.0); 

    // get input for SSAO algorithm
    vec3 fragPos = (view * vec4(texture(g_position_texture_, tex_coords).xyz, 1.0)).xyz;
    vec3 normal = normalize((transpose(inverse(view)) * vec4(texture(g_normal_texture_, tex_coords).rgb, 1.0)).xyz);
    vec3 randomVec = normalize(texture(ssao_noise_texture_, tex_coords * noiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = (view * vec4(texture(g_position_texture_, offset.xy).xyz, 1.0)).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    occlusion_factor = occlusion;
}

)glsl";


const std::string lighting_pass_vshader = R"glsl(
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texture_coordinates;

out vec2 TexCoords;

void main()
{
    TexCoords = texture_coordinates;
    gl_Position = vec4(position, 0.0, 1.0);
}
)glsl";


const std::string lighting_pass_fshader = R"glsl(
#version 330 core

in vec2 TexCoords;

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
  vec3 in_world_position = texture(g_position_texture_, TexCoords).rgb;
  vec3 normal = texture(g_normal_texture_, TexCoords).rgb;
  vec3 voxel_color = texture(g_color_texture_, TexCoords).rgb;
  float voxel_reflectivity = texture(g_material_texture_, TexCoords).r;
  float voxel_shininess = texture(g_material_texture_, TexCoords).g;
  float voxel_opacity = texture(g_material_texture_, TexCoords).b;
  float ambient_occlusion = texture(ssao_texture_, TexCoords).r;
  
  // Calculate sunlight phong vector
  // Calculate ambient light
  vec3 sunlight_ambient = sunlight.ambient * voxel_color;
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

  //gl_FragColor = vec4(phong, voxel_opacity);
  gl_FragColor = vec4(0.0);
  if (voxel_opacity != 0.0) gl_FragColor = vec4(vec3(ambient_occlusion), 1.0);
}
)glsl";
































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
