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

layout(set = 1, binding = 0) uniform sampler2D image;

layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;

void main() {
  vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
  vec3 specularLight = vec3(0.0);
  vec3 surfaceNormal = normalize(fragNormalWorld);

  vec3 cameraPosWorld = ubo.invView[3].xyz;
  vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

  for (int i = 0; i < ubo.numPointLights; i++) {
    PointLight light = ubo.pointLights[i];
    vec3 directionToLight = light.position.xyz - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
    directionToLight = normalize(directionToLight);

    float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
    vec3 intensity = light.color.xyz * light.color.w * attenuation;

    diffuseLight += intensity * cosAngIncidence;

    // specular lighting
    vec3 halfAngle = normalize(directionToLight + viewDirection);
    float blinnTerm = dot(surfaceNormal, halfAngle);
    blinnTerm = clamp(blinnTerm, 0, 1);
    blinnTerm = pow(blinnTerm, 512.0); // higher values -> sharper highlight
    specularLight += intensity * blinnTerm;
  }

  for (int i = 0; i < ubo.numDirectionalLights; i++) {
    DirectionalLight light = ubo.directionalLights[i];
    vec3 directionToLight = -light.direction.xyz;

    float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
    vec3 intensity = light.color.xyz * light.color.w;

    diffuseLight += intensity * cosAngIncidence;
//    diffuseLight = vec3(light.direction.xyz);
  }

  vec3 imageColor = texture(image, fragUV).rgb;
  outColor = vec4(diffuseLight * fragColor * imageColor + specularLight * fragColor * imageColor, 1.0);
  outColor = vec4(diffuseLight, 1);
//  outColor = vec4(t);
  //outColor = vec4(fragUV.r, fragUV.g, 0);
}
