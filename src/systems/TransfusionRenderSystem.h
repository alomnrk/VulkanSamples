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
class TransfusionRenderSystem {
 public:
  TransfusionRenderSystem(
          Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout textureSetLayout, AssetsSystem &assetSystem);
  ~TransfusionRenderSystem();

  TransfusionRenderSystem(const TransfusionRenderSystem &) = delete;
  TransfusionRenderSystem &operator=(const TransfusionRenderSystem &) = delete;

  void renderGameObjects(FrameInfo &frameInfo, GameObject::Map &gameObjects);

 private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout textureSetLayout);
  void createPipeline(VkRenderPass renderPass);

  Device &device;
  AssetsSystem &assetSystem;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
};
}  // namespace lwmeta
