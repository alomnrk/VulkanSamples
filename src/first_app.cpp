#include "first_app.h"

#include "keyboard_movement_controller.h"
#include "buffer.h"
#include "camera.h"
#include "systems/point_light_system.h"
#include "systems/simple_render_system.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>


#include "Texture.h"


namespace lwmeta {

FirstApp::FirstApp() {
    globalPool =
      DescriptorPool::Builder(device)
          .setMaxSets(SwapChain::MAX_DESCRIPTION_SETS)
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_TEXTURES)
          .build();

    textureSetLayout = DescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

    loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < uboBuffers.size(); i++) {
    uboBuffers[i] = std::make_unique<Buffer>(
            device,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    uboBuffers[i]->map();
  }

  auto globalSetLayout =
      DescriptorSetLayout::Builder(device)
          .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
          .build();



  std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < globalDescriptorSets.size(); i++) {
    auto bufferInfo = uboBuffers[i]->descriptorInfo();
    DescriptorWriter(*globalSetLayout, *globalPool)
        .writeBuffer(0, &bufferInfo)
        .build(globalDescriptorSets[i]);
  }



  SimpleRenderSystem simpleRenderSystem{
          device,
          renderer.getSwapChainRenderPass(),
          globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout(), assetSystem};
  PointLightSystem pointLightSystem{
          device,
          renderer.getSwapChainRenderPass(),
          globalSetLayout->getDescriptorSetLayout()};
  Camera camera{};

  auto viewerObject = GameObject::createGameObject();
  viewerObject.transform.translation.z = -2.5f;
  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();
  while (!window.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    float aspect = renderer.getAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

    if (auto commandBuffer = renderer.beginFrame()) {
      int frameIndex = renderer.getFrameIndex();
      FrameInfo frameInfo{
          frameIndex,
          frameTime,
          commandBuffer,
          camera,
          globalDescriptorSets[frameIndex],
          gameObjects};

      // update
      GlobalUbo ubo{};
      ubo.projection = camera.getProjection();
      ubo.view = camera.getView();
      ubo.inverseView = camera.getInverseView();
      pointLightSystem.update(frameInfo, ubo);
      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      // render
      renderer.beginSwapChainRenderPass(commandBuffer);

      // order here matters
      simpleRenderSystem.renderGameObjects(frameInfo);
      pointLightSystem.render(frameInfo);

      renderer.endSwapChainRenderPass(commandBuffer);
      renderer.endFrame();
    }
  }

  vkDeviceWaitIdle(device.device());
}


void FirstApp::loadGameObjects() {
    auto testTextureId = assetSystem.AddTexture("../textures/test2.jpg");
    auto testMaterialId = assetSystem.CreateMaterial(testTextureId, textureSetLayout.get(), globalPool.get());

    auto testTexture2Id = assetSystem.AddTexture("../textures/test.png");
    auto testMaterial2Id = assetSystem.CreateMaterial(testTexture2Id, textureSetLayout.get(), globalPool.get());

  std::shared_ptr<Model> model =
      Model::createModelFromFile(device, "models/flat_vase.obj");
  auto flatVase = GameObject::createGameObject();
  flatVase.model = model;
  flatVase.transform.translation = {-.5f, .5f, 0.f};
  flatVase.transform.scale = {3.f, 1.5f, 3.f};
  flatVase.materialId = testMaterialId; //material
  gameObjects.emplace(flatVase.getId(), std::move(flatVase));

  model = Model::createModelFromFile(device, "models/smooth_vase.obj");
  auto smoothVase = GameObject::createGameObject();
  smoothVase.model = model;
  smoothVase.transform.translation = {.5f, .5f, 0.f};
  smoothVase.transform.scale = {3.f, 1.5f, 3.f};
    smoothVase.materialId = testMaterial2Id; //material
  gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));
//
//  model = Model::createModelFromFile(device, "models/quad.obj");
//  auto floor = GameObject::createGameObject();
//  floor.model = model;
//  floor.transform.translation = {0.f, .5f, 0.f};
//  floor.transform.scale = {3.f, 1.f, 3.f};
//  gameObjects.emplace(floor.getId(), std::move(floor));

  std::vector<glm::vec3> lightColors{
      {1.f, .1f, .1f},
      {.1f, .1f, 1.f},
      {.1f, 1.f, .1f},
      {1.f, 1.f, .1f},
      {.1f, 1.f, 1.f},
      {1.f, 1.f, 1.f}  //
  };

  for (int i = 0; i < lightColors.size(); i++) {
    auto pointLight = GameObject::makePointLight(0.2f);
    pointLight.color = lightColors[i];
    auto rotateLight = glm::rotate(
        glm::mat4(1.f),
        (i * glm::two_pi<float>()) / lightColors.size(),
        {0.f, -1.f, 0.f});
    pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
    gameObjects.emplace(pointLight.getId(), std::move(pointLight));
  }
}

}  // namespace lwmeta
