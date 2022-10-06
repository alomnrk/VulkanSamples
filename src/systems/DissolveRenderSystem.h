#pragma once

#include "../camera.h"
#include "../device.h"
#include "../frame_info.h"
#include "../game_object.h"
#include "../pipeline.h"

// std
#include <memory>
#include <vector>
#include "../AssetsSystem.h"

namespace lwmeta {
class DissolveRenderSystem {
 public:
  DissolveRenderSystem(
          Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout textureSetLayout, AssetsSystem &assetSystem);
  ~DissolveRenderSystem();

  DissolveRenderSystem(const DissolveRenderSystem &) = delete;
  DissolveRenderSystem &operator=(const DissolveRenderSystem &) = delete;

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