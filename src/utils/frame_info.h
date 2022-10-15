#pragma once

#include "camera.h"
//#include "game_object.h"

// lib
#include <vulkan/vulkan.h>
#include "../logic/LightSystem.h"
#include "camera.h"

#include <iostream>

namespace lwmeta {

#define MAX_LIGHTS 10

    struct GlobalUbo {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::mat4 inverseView{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .09f};  // w is intensity
        DirectionalLight directionalLights[MAX_LIGHTS];
        PointLight pointLights[MAX_LIGHTS];
        int numDirectionalLights;
        int numPointLights;
        float time;


        void UpdateCamera(Camera &camera){
            projection = camera.getProjection();
            view = camera.getView();
            inverseView = camera.getInverseView();
        }

        void UpdateLights(LightSystem lightSystem) {
            numDirectionalLights = lightSystem.directionalLights.size();

            uint32_t lightIndex = 0;
            for (auto pl: lightSystem.directionalLights) {
//                std::cout << pl->position.x << std::endl;
                directionalLights[lightIndex] = *pl;
                lightIndex++;
            }

            numPointLights = lightSystem.pointLights.size();

            lightIndex = 0;
            for (auto pl: lightSystem.pointLights) {
//                std::cout << pl->position.x << std::endl;
                pointLights[lightIndex] = *pl;
                lightIndex++;
            }
        }
    };

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera &camera;
        VkDescriptorSet globalDescriptorSet;
    };
}  // namespace lwmeta
