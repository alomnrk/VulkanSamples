#include "first_app.h"

#include "../utils/keyboard_movement_controller.h"
#include "../vulkan_utils/buffer.h"
#include "../utils/camera.h"
#include "../systems/point_light_system.h"
#include "../systems/LitRenderSystem.h"
#include "../systems/SkyBoxRenderSystem.h"
#include "../systems/BlackHoleRenderSystem.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "../systems/TransfusionRenderSystem.h"
#include "../systems/DissolveRenderSystem.h"

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>


#include "../assets/Texture.h"




namespace lwmeta {

    FirstApp::FirstApp() {
        globalPool =
                DescriptorPool::Builder(device)
                        .setMaxSets(SwapChain::MAX_DESCRIPTION_SETS)
                        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100)
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
//        auto testMaterialId = assetSystem.CreateCubeMapMaterial(cubeMapId, textureSetLayout.get(), globalPool.get());

        auto diskTexture = assetSystem.AddTexture("../textures/space.jpg", 4, true);
//        auto diskMaterial = assetSystem.CreateTextureMaterial(diskTexture, 1, textureSetLayout.get(), globalPool.get());

//        auto blacHoleMat = assetSystem.CreateBlackHoleMaterial(cubeMapId, diskTexture, textureSetLayout.get(),
//                                                               globalPool.get());


        auto skybox = GameObject::createGameObjectPtr();
//        skybox->AddComponent(new MaterialComponent(blacHoleMat));
        scene = new Scene(skybox);
//        loadGameObjects();
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


//        LitRenderSystem simpleRenderSystem{
//                device,
//                renderer.getSwapChainRenderPass(),
//                globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout(), assetSystem};

//        TransfusionRenderSystem transfusionRenderSystem{
//                device,
//                renderer.getSwapChainRenderPass(),
//                globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout(), assetSystem};


//        DissolveRenderSystem dissolveRenderSystem{
//                device,
//                renderer.getSwapChainRenderPass(),
//                globalSetLayout->getDescriptorSetLayout(), assetSystem};
//
//
//        PointLightSystem pointLightSystem{
//                device,
//                renderer.getSwapChainRenderPass(),
//                globalSetLayout->getDescriptorSetLayout()};

//    SkyBoxRenderSystem skyBoxRenderSystem{
//            device,
//            renderer.getSwapChainRenderPass(),
//            globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout(), assetSystem};

//        BlackHoleRenderSystem blackHoleRenderSystem{
//                device,
//                renderer.getSwapChainRenderPass(),
//                globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout(), assetSystem};


        loadGameObjects(globalSetLayout->getDescriptorSetLayout(), renderer.getSwapChainRenderPass());

        Camera camera{};

        auto viewerObject = GameObject::createGameObject();
        viewerObject.transform->translation.z = -2.5f;
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
            camera.setViewYXZ(viewerObject.transform->translation, viewerObject.transform->rotation);

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
                // pointLightSystem.update(frameInfo, ubo, scene->pointLights);
                MoveLights(frameTime);

                GlobalUbo ubo{};
                ubo.UpdateCamera(camera);
                ubo.UpdateLights(scene->lightSystem);
                ubo.time = std::chrono::duration<float, std::chrono::seconds::period>(
                        std::chrono::high_resolution_clock::now() - currentTime2).count();

                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                renderer.beginSwapChainRenderPass(commandBuffer);

                // order here matters
//                blackHoleRenderSystem.renderGameObjects(frameInfo, *scene->skyBox);
//                dynamic_cast<DissolveLitMaterial*>(assetSystem.GetMaterial(scene->player->GetComponent<MaterialComponent>()->materialId))->UpdateValues((glm::sin(ubo.time * 2.0) + 1) / 2.0f );
//                simpleRenderSystem.renderGameObjects(frameInfo, scene->litObjects);
//                pointLightSystem.render(frameInfo, scene->pointLights);



                auto g1 = &scene->litObjects.at(0);
                dynamic_cast<LitMaterialInstance*>(g1->GetComponent<MaterialComponent>()->getMaterialInstance())->UpdatePush(g1->transform->mat4(), g1->transform->normalMatrix());

                auto g2 = &scene->litObjects.at(1);
                dynamic_cast<PBRMaterialInstance*>(g2->GetComponent<MaterialComponent>()->getMaterialInstance())->UpdatePush(g2->transform->mat4(), g2->transform->normalMatrix());

                scene->litObjects.at(0).Update();
                scene->litObjects.at(1).Update();

                //global desc and command buffer
                scene->litObjects.at(0).Render(frameInfo);
                scene->litObjects.at(1).Render(frameInfo);

                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(device.device());
    }

    void FirstApp::MoveLights(float time){
        auto rotateLight = glm::rotate(glm::mat4(1.f), 0.5f * time, {0.f, -1.f, 0.f});
        int lightIndex = 0;
        for (auto &kv: scene->pointLights) {
            auto &obj = kv.second;

            assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");

            // update light position
//            std::cout << glm::vec3(rotateLight * glm::vec4(obj.transform->translation, 1.f)).x << std::endl;
            obj.transform->translation = glm::vec3(rotateLight * glm::vec4(obj.transform->translation, 1.f));

            obj.Update();
        }
    }




    void FirstApp::AddLights(){
        std::vector<glm::vec3> lightColors{
                {1.f, .1f, .1f},
                {.1f, .1f, 1.f},
                {.1f, 1.f, .1f},
                {1.f, 1.f, .1f},
                {.1f, 1.f, 1.f},
                {1.f, 1.f, 1.f}
        };
//
        for (int i = 0; i < lightColors.size(); i++) {
            auto pointLight = GameObject::createGameObject();
            auto plComponent = new PointLightComponent(&scene->lightSystem, 2, lightColors[i], pointLight.transform.get());
            pointLight.AddComponent<PointLightComponent>(plComponent);

            pointLight.AddComponent<PointLightRendererComponent>(new PointLightRendererComponent(0.1f, plComponent));
            auto rotateLight = glm::rotate(
                    glm::mat4(1.f),
                    (i * glm::two_pi<float>()) / lightColors.size(),
                    {0.f, -1.f, 0.f});
            pointLight.transform->translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
            scene->AddPointLight(std::move(pointLight));
        }

        //Add Directional
        auto directionalLight = GameObject::createGameObject();
        auto dlComponent = new DirectionalLightComponent(&scene->lightSystem, 2, lightColors[0], directionalLight.transform.get());
        directionalLight.AddComponent<DirectionalLightComponent>(dlComponent);
        directionalLight.transform->rotation = glm::vec3{0, 3.14 / 2.0, 0};
        scene->AddPointLight(std::move(directionalLight));
    }

    void FirstApp::loadGameObjects(VkDescriptorSetLayout globalSetLayout, VkRenderPass renderPass) {
        auto testTextureId = assetSystem.AddTexture("../textures/Env_Base_Color_1001.png", 4, true);
        materialSystem.CreateLitMaterial(device, globalSetLayout, renderPass);
        uint32_t testMaterialId = materialSystem.CreateLitMaterialInstance(assetSystem.GetTexture(testTextureId),
                                                                           globalPool.get());


        uint32_t testModelId = assetSystem.AddModel(Model::createModelFromFile(device, "models/flat_vase.obj"));
        auto testModel = GameObject::createGameObject();
        auto matc = new MaterialComponent(&materialSystem, "Lit", testMaterialId);
        testModel.AddComponent<MaterialComponent>(matc);
        auto mc = new MeshComponent(&assetSystem, testModelId);
        testModel.AddComponent<MeshComponent>(mc);
        testModel.AddComponent<UsualRendererComponent>(new UsualRendererComponent(0, mc, matc));

        testModel.transform->translation = {-0.5f, .5f, 0.f};
        testModel.transform->rotation = {0, 0, 0};
        testModel.transform->scale = {1, 1, 1};
//        scene->player = std::make_unique<GameObject>(std::move(flatVase));
        scene->AddLitObject(std::move(testModel));


        testTextureId = assetSystem.AddTexture("../textures/Env_Base_Color_1001.png", 4, false);

        //pbr
        auto albedoTextureId = assetSystem.AddTexture("../textures/pbr/obsidian/Base_Color.png", 4, true);
//        auto albedoTextureId = assetSystem.AddTexture("../textures/pbr/test/rustediron2_basecolor.png", 4, true);

        auto normalTextureId = assetSystem.AddTexture("../textures/pbr/obsidian/Normal.png", 4, false);
        auto metallicTextureId = assetSystem.AddTexture("../textures/pbr/obsidian/Metallic.png", 1, false);
        auto roughnessTextureId = assetSystem.AddTexture("../textures/pbr/obsidian/Roughness.png", 1, false);

        materialSystem.CreatePBRMaterial(device, globalSetLayout, renderPass);
        auto testMaterialId2 = materialSystem.CreatePBRMaterialInstance(assetSystem.GetTexture(albedoTextureId), assetSystem.GetTexture(normalTextureId), assetSystem.GetTexture(metallicTextureId), assetSystem.GetTexture(roughnessTextureId),
                                                                  globalPool.get());


        testModelId = assetSystem.AddModel(Model::createModelFromFile(device, "models/head_v1.obj"));
        testModel = GameObject::createGameObject();
        matc = new MaterialComponent(&materialSystem, "PBR", testMaterialId2);
        testModel.AddComponent<MaterialComponent>(matc);
        mc = new MeshComponent(&assetSystem, testModelId);
        testModel.AddComponent<MeshComponent>(mc);
        testModel.AddComponent<UsualRendererComponent>(new UsualRendererComponent(0, mc, matc));

        testModel.transform->translation = {0.5f, .5f, 0.f};
        testModel.transform->rotation = {0, 0, 0};
        testModel.transform->scale = {1, 1, 1};
//        scene->player = std::make_unique<GameObject>(std::move(flatVase));
        scene->AddLitObject(std::move(testModel));


        AddLights();
    }

}  // namespace lwmeta
