//
// Created by Artem Ustinov on 10.10.2022.
//

#ifndef LWMETAVERSE_FractalMaterialInstance_H
#define LWMETAVERSE_FractalMaterialInstance_H

#include <vulkan/vulkan.h>
#include "MaterialSystem.h"

namespace lwmeta {
    struct FractalMaterialInstance : MaterialInstance {
    public:
        FractalMaterialInstance(uint32_t id, DescriptorSetLayout &layout, DescriptorPool *pool, VkPipelineLayout &pipelineLayout) : MaterialInstance(
                id, "Fractal", pipelineLayout) {

            std::cout << "create Fractal material" << std::endl;
            std::cout << "finish create PBR material" << std::endl;
        }

        void bind(FrameInfo &frameInfo) override {
            vkCmdPushConstants(
                    frameInfo.commandBuffer,
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(BasePushConstantData),
                    &push);
        }

        void UpdatePush(glm::mat4 modelMatrix, glm::mat4 normalMatrix) {
            push.modelMatrix = modelMatrix;
            push.normalMatrix = normalMatrix;
        }

    private:
//        VkDescriptorSet materialSet;
        BasePushConstantData push;
    };
}


#endif //LWMETAVERSE_FractalMaterialInstance_H
