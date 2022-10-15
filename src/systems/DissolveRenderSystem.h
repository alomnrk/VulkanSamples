#pragma once

#include "../utils/camera.h"
#include "../vulkan_utils/device.h"
#include "../utils/frame_info.h"
#include "../logic/game_object.h"
#include "../vulkan_utils/pipeline.h"

// std
#include <memory>
#include <vector>
#include "../assets/AssetsSystem.h"

namespace lwmeta {
    class DissolveRenderSystem {
    public:
        DissolveRenderSystem(
                Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout,
                AssetsSystem &assetSystem);

        ~DissolveRenderSystem();

        DissolveRenderSystem(const DissolveRenderSystem &) = delete;

        DissolveRenderSystem &operator=(const DissolveRenderSystem &) = delete;

        void renderGameObjects(FrameInfo &frameInfo, GameObject::Map &gameObjects);
        void renderGameObject(FrameInfo &frameInfo, GameObject &gameObject);


        std::unique_ptr<DescriptorSetLayout> materialSetLayout;
    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createMaterialLayout();
        void createPipeline(VkRenderPass renderPass);

        Device &device;
        AssetsSystem &assetSystem;

        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;


    };
}  // namespace lwmeta
