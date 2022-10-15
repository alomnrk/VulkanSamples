#pragma once

#include "../utils/camera.h"
#include "../vulkan_utils/device.h"
#include "../utils/frame_info.h"
//#include "../game_object.h"
#include "../vulkan_utils/pipeline.h"
#include "../vulkan_utils/descriptors.h"
// std
#include <memory>
#include <vector>
//#include "../AssetsSystem.h"

namespace lwmeta {
    class UniversalRenderSystem {
    public:
        UniversalRenderSystem(
                Device &device, std::string vertPath, std::string fragPath, VkPipelineLayout &pipelineLayout, VkRenderPass renderPass);

        ~UniversalRenderSystem();

        UniversalRenderSystem(const UniversalRenderSystem &) = delete;

        UniversalRenderSystem &operator=(const UniversalRenderSystem &) = delete;

        void bind(FrameInfo &frameInfo);

//        void renderGameObjects(FrameInfo &frameInfo, GameObject::Map &gameObjects);
//        void renderGameObject(FrameInfo &frameInfo, GameObject &gameObject);


        std::unique_ptr<DescriptorSetLayout> materialSetLayout;
    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout,  VkDescriptorSetLayout materialSetLayout, VkPushConstantRange pushConstantRange);
//        void createMaterialLayout(VkDescriptorSetLayout materialSetLayout);
        void createPipeline(std::string vertPath, std::string fragPath, VkRenderPass renderPass);

        Device &device;
//        AssetsSystem &assetSystem;

        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout &pipelineLayout;
    };
}  // namespace lwmeta
