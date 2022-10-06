#include "first_app.h"

#include "keyboard_movement_controller.h"
#include "buffer.h"
#include "camera.h"
#include "systems/point_light_system.h"
#include "systems/LitRenderSystem.h"
#include "systems/SkyBoxRenderSystem.h"
#include "systems/BlackHoleRenderSystem.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "systems/TransfusionRenderSystem.h"

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
                .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();

//    texture2SetLayout = DescriptorSetLayout::Builder(device)
//            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
//            .build();



        auto cubeMapId = assetSystem.AddCubeMap({
                                                        "../textures/skyboxes/milkyway/right.jpg",
                                                        "../textures/skyboxes/milkyway/left.jpg",
                                                        "../textures/skyboxes/milkyway/up.jpg",
                                                        "../textures/skyboxes/milkyway/bottom.jpg",
                                                        "../textures/skyboxes/milkyway/far.jpg",
                                                        "../textures/skyboxes/milkyway/near.jpg",
                                                });
        auto testMaterialId = assetSystem.CreateCubeMapMaterial(cubeMapId, textureSetLayout.get(), globalPool.get());

        auto diskTexture = assetSystem.AddTexture("../textures/space.jpg");
        auto diskMaterial = assetSystem.CreateTextureMaterial(diskTexture, 1, textureSetLayout.get(), globalPool.get());

        auto blacHoleMat = assetSystem.CreateBlackHoleMaterial(cubeMapId, diskTexture, textureSetLayout.get(),
                                                               globalPool.get());


        auto skybox = GameObject::createGameObjectPtr();
        skybox->AddComponent(new MaterialComponent(blacHoleMat));
        scene = new Scene(skybox);
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


        LitRenderSystem simpleRenderSystem{
                device,
                renderer.getSwapChainRenderPass(),
                globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout(), assetSystem};

        TransfusionRenderSystem transfusionRenderSystem{
                device,
                renderer.getSwapChainRenderPass(),
                globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout(), assetSystem};


        PointLightSystem pointLightSystem{
                device,
                renderer.getSwapChainRenderPass(),
                globalSetLayout->getDescriptorSetLayout()};

//    SkyBoxRenderSystem skyBoxRenderSystem{
//            device,
//            renderer.getSwapChainRenderPass(),
//            globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout(), assetSystem};

        BlackHoleRenderSystem blackHoleRenderSystem{
                device,
                renderer.getSwapChainRenderPass(),
                globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout(), assetSystem};


        Camera camera{};

        auto viewerObject = GameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto currentTime2 = std::chrono::high_resolution_clock::now();
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
                       };

                // update
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
                ubo.time = std::chrono::duration<float, std::chrono::seconds::period>(
                        std::chrono::high_resolution_clock::now() - currentTime2).count();
                pointLightSystem.update(frameInfo, ubo, scene->pointLights);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
                // render
                renderer.beginSwapChainRenderPass(commandBuffer);

                // order here matters
//                blackHoleRenderSystem.renderGameObjects(frameInfo, *scene->skyBox);
                transfusionRenderSystem.renderGameObjects(frameInfo, scene->litObjects);
      pointLightSystem.render(frameInfo, scene->pointLights);


                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(device.device());
    }


    void FirstApp::loadGameObjects() {
        auto testTextureId = assetSystem.AddTexture("../textures/alfa_head_V3.1.png");
        auto testMaterialId = assetSystem.CreateLitMaterial(testTextureId, textureSetLayout.get(), globalPool.get());

        auto testTexture2Id = assetSystem.AddTexture("../textures/test.png");
        auto testMaterial2Id = assetSystem.CreateLitMaterial(testTexture2Id, textureSetLayout.get(), globalPool.get());

        uint32_t vaseFlatModelId = assetSystem.AddModel(Model::createModelFromFile(device, "models/head_v1.obj"));
//        uint32_t vaseFlatModelId = assetSystem.AddModel(Model::createModelFromFile(device, "models/body_v1.obj"));
        auto flatVase = GameObject::createGameObject();
        flatVase.AddComponent<MaterialComponent>(new MaterialComponent(testMaterialId));
        flatVase.AddComponent<MeshComponent>(new MeshComponent(vaseFlatModelId));
        flatVase.transform.translation = {-.5f, .5f, 0.f};
        flatVase.transform.scale = {1, 1, 1};
        scene->AddLitObject(std::move(flatVase));

        uint32_t vaseSmoothModelId = assetSystem.AddModel(Model::createModelFromFile(device, "models/smooth_vase.obj"));
        auto smoothVase = GameObject::createGameObject();
        smoothVase.AddComponent<MaterialComponent>(new MaterialComponent(testMaterial2Id));
        smoothVase.AddComponent<MeshComponent>(new MeshComponent(vaseSmoothModelId));
        smoothVase.transform.translation = {.5f, .5f, 0.f};
        smoothVase.transform.scale = {3.f, 1.5f, 3.f};
        scene->AddLitObject(std::move(smoothVase));

        std::vector<glm::vec3> lightColors{
                {1.f, .1f, .1f},
                {.1f, .1f, 1.f},
                {.1f, 1.f, .1f},
                {1.f, 1.f, .1f},
                {.1f, 1.f, 1.f},
                {1.f, 1.f, 1.f}  //
        };

        for (int i = 0; i < lightColors.size(); i++) {
            auto pointLight = GameObject::createGameObject();
            auto plComponent = new PointLightComponent(0.2f, lightColors[i]);
            pointLight.AddComponent<PointLightComponent>(plComponent);

            pointLight.AddComponent<PointLightRendererComponent>(new PointLightRendererComponent(0.1f, plComponent));
            auto rotateLight = glm::rotate(
                    glm::mat4(1.f),
                    (i * glm::two_pi<float>()) / lightColors.size(),
                    {0.f, -1.f, 0.f});
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
            scene->AddPointLight(std::move(pointLight));
        }
    }

}  // namespace lwmeta
