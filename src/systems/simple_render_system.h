#pragma once

#include "../camera.h"
#include "../device.h"
#include "../frame_info.h"
#include "../game_object.h"
#include "../pipeline.h"

// std
#include <memory>
#include <vector>

namespace lwmeta {
class SimpleRenderSystem {
 public:
  SimpleRenderSystem(
          Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(FrameInfo &frameInfo);

 private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void createPipeline(VkRenderPass renderPass);

  Device &device;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
};
}  // namespace lwmeta
