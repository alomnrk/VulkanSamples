//
// Created by Artem Ustinov on 08.10.2022.
//
//#pragma once
#ifndef LWMETAVERSE_MATERIAL_H
#define LWMETAVERSE_MATERIAL_H

#include <cstdint>
#include "../systems/UniversalRenderSystem.h"
#include "../utils/frame_info.h"
#include "../assets/Texture.h"
#include "../vulkan_utils/buffer.h"

#include "MaterialInstance.h"

#include <iostream>

namespace lwmeta {
    class Material {
    public:

        Material(Device &device, std::string vertPath, std::string fragPath, VkDescriptorSetLayout globalSetLayout,
                 std::shared_ptr<DescriptorSetLayout> materialSetLayout,
                 VkPushConstantRange pushConstantRange, VkRenderPass renderPass) {
            this->materialSetLayout = materialSetLayout;
            createPipelineLayout(device, globalSetLayout, materialSetLayout->getDescriptorSetLayout(), pushConstantRange);
            universalRenderSystem = std::make_unique<UniversalRenderSystem>(device, vertPath, fragPath, pipelineLayout, renderPass);
        };

//        ~Material();

        uint32_t AddInstance(MaterialInstance* instance){
            auto id = currentId++;
            materialsInstances.emplace(id, instance);
            return id;
        }

        DescriptorSetLayout *getMaterialSetLayout() { return materialSetLayout.get(); };

        MaterialInstance* GetMaterialInstance(uint32_t instance_id){
            return materialsInstances.at(instance_id);
        }

        void bind(FrameInfo &frameInfo){
            universalRenderSystem->bind(frameInfo);
        }

        VkPipelineLayout pipelineLayout;
    private:
        void createPipelineLayout(Device &device, VkDescriptorSetLayout globalSetLayout,
                                  VkDescriptorSetLayout materialSetLayout, VkPushConstantRange pushConstantRange) {
//            VkPushConstantRange pushConstantRange{};
//            pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
//            pushConstantRange.offset = 0;
//            pushConstantRange.size = sizeof(SimplePushConstantData);

            std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout, materialSetLayout};

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
            pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
            pipelineLayoutInfo.pushConstantRangeCount = 1;
            pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
            if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }
        }


        uint32_t currentId;

        std::unique_ptr<UniversalRenderSystem> universalRenderSystem;
        std::shared_ptr<DescriptorSetLayout> materialSetLayout;


        std::unordered_map<uint32_t, MaterialInstance *> materialsInstances;
    };
}

#endif //LWMETAVERSE_MATERIAL_H
