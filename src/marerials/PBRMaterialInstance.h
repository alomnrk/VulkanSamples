//
// Created by Artem Ustinov on 10.10.2022.
//

#ifndef LWMETAVERSE_PBRMATERIALINSTANCE_H
#define LWMETAVERSE_PBRMATERIALINSTANCE_H

#include <vulkan/vulkan.h>
#include "MaterialSystem.h"

namespace lwmeta {
    struct PBRMaterialInstance : MaterialInstance {
    public:
        PBRMaterialInstance(uint32_t id, DescriptorSetLayout &layout, Texture *albedoTexture, Texture* normalTexture, Texture *metallicTexture, Texture *roughnessTexture,
                                                 DescriptorPool *pool, VkPipelineLayout &pipelineLayout) : MaterialInstance(
                id, "PBR", pipelineLayout) {
            VkDescriptorImageInfo albedoInfo;
            albedoInfo.sampler = albedoTexture->getSampler();
            albedoInfo.imageView = albedoTexture->getImageView();
            albedoInfo.imageLayout = albedoTexture->getImageLayout();

            VkDescriptorImageInfo normalInfo;
            normalInfo.sampler = normalTexture->getSampler();
            normalInfo.imageView = normalTexture->getImageView();
            normalInfo.imageLayout = normalTexture->getImageLayout();

            VkDescriptorImageInfo metallicInfo;
            metallicInfo.sampler = metallicTexture->getSampler();
            metallicInfo.imageView = metallicTexture->getImageView();
            metallicInfo.imageLayout = metallicTexture->getImageLayout();

            VkDescriptorImageInfo roughnessInfo;
            roughnessInfo.sampler = roughnessTexture->getSampler();
            roughnessInfo.imageView = roughnessTexture->getImageView();
            roughnessInfo.imageLayout = roughnessTexture->getImageLayout();

//            VkDescriptorImageInfo aoInfo;
//            aoInfo.sampler = aoTexture->getSampler();
//            aoInfo.imageView = aoTexture->getImageView();
//            aoInfo.imageLayout = aoTexture->getImageLayout();

            std::cout << "create PBR material" << std::endl;
            DescriptorWriter(layout, *pool)
                    .writeImage(0, &albedoInfo)
                    .writeImage(1, &normalInfo)
                    .writeImage(2, &metallicInfo)
                    .writeImage(3, &roughnessInfo)
//                    .writeImage(4, &aoInfo)
                    .build(materialSet);
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


#endif //LWMETAVERSE_PBRMATERIALINSTANCE_H
