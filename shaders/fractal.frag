#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

struct PointLight {
  vec4 position; // ignore w
  vec4 color; // w is intensity
};

struct DirectionalLight {
  vec4 direction; // ignore w
  vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  vec4 ambientLightColor; // w is intensity
  DirectionalLight directionalLights[10];
  PointLight pointLights[10];
  int numDirectionalLights;
  int numPointLights;
} ubo;

//layout(set = 1, binding = 0) uniform sampler2D albedoTexture;
//layout(set = 1, binding = 1) uniform sampler2D normalTexture;
//layout(set = 1, binding = 2) uniform sampler2D metallicTexture;
//layout(set = 1, binding = 3) uniform sampler2D roughnessTexture;
//layout(set = 1, binding = 4) uniform sampler2D aoTexture;

layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;

const float PI = 3.14159265359;
//const int MaximumRaySteps = 10;
const int Iterations = 8;
const float Scale = 2;
const float Offset = 1;

float DistanceEstimator(vec3 z)
{
  float r;
  int n = 0;
  while (n < Iterations) {
    if(z.x+z.y<0) z.xy = -z.yx; // fold 1
    if(z.x+z.z<0) z.xz = -z.zx; // fold 2
    if(z.y+z.z<0) z.zy = -z.yz; // fold 3
    z = z*Scale - Offset*(Scale-1.0);
    n++;
  }
  return (length(z) ) * pow(Scale, -float(n));
}

float trace(vec3 from, vec3 direction) {
  float MinimumDistance = 0.02f;
//  int MaximumRaySteps = 10;

  float totalDistance = 0.0;
  int steps;
  for (steps=0; steps < 12; steps++) {
    vec3 p = from + totalDistance * direction;
    float distance = DistanceEstimator(p);
    totalDistance += distance;
    if (distance < MinimumDistance) break;
  }
  return 1.0-float(steps)/float(12);
}

void main() {
  vec3 cameraPosWorld = ubo.invView[3].xyz;
  vec3 direction = normalize(fragPosWorld - cameraPosWorld);
  float y = trace(cameraPosWorld, direction);


  outColor = vec4(vec3(y), 1.0);
}
