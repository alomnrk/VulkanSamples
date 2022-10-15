//
// Created by Artem Ustinov on 10.10.2022.
//

#ifndef LWMETAVERSE_LITMATERIALINSTANCE_H
#define LWMETAVERSE_LITMATERIALINSTANCE_H

#include <vulkan/vulkan.h>
#include "MaterialSystem.h"

namespace lwmeta {
    struct LitMaterialInstance : MaterialInstance {
    public:
        LitMaterialInstance(uint32_t id, DescriptorSetLayout &layout, Texture *texture,
                                                 DescriptorPool *pool, VkPipelineLayout &pipelineLayout) : MaterialInstance(
                id, "Lit", pipelineLayout) {
            VkDescriptorImageInfo imageInfo;
            imageInfo.sampler = texture->getSampler();
            imageInfo.imageView = texture->getImageView();
            imageInfo.imageLayout = texture->getImageLayout();

            std::cout << "create Lit material" << std::endl;
            DescriptorWriter(layout, *pool)
                    .writeImage(0, &imageInfo)
                    .build(materialSet);
            std::cout << "finish create Lit material" << std::endl;
        }

        void bind(FrameInfo &frameInfo) override {
            vkCmdPushConstants(
                    frameInfo.commandBuffer,
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(BasePushConstantData),
                    &push);

            vkCmdBindDescriptorSets(
                    frameInfo.commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipelineLayout,
                    1,
                    1,
                    &materialSet,
                    0,
                    nullptr);
        }

        void UpdatePush(glm::mat4 modelMatrix, glm::mat4 normalMatrix) {
            push.modelMatrix = modelMatrix;
            push.normalMatrix = normalMatrix;
        }

    private:
        VkDescriptorSet materialSet;
        BasePushConstantData push;

    };
}


#endif //LWMETAVERSE_LITMATERIALINSTANCE_H
