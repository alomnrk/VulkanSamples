#pragma once

#include "../vulkan_utils/descriptors.h"
#include "../vulkan_utils/device.h"
#include "game_object.h"
#include "../utils/renderer.h"
#include "../vulkan_utils/window.h"
#include "../assets/AssetsSystem.h"
#include "Scene.h"
#include "../systems/DissolveRenderSystem.h"
#include "../marerials/Material.h"
#include "../marerials/LitMaterialInstance.h"
#include "../marerials/PBRMaterialInstance.h"
// std
#include <memory>
#include <vector>

namespace lwmeta {
    class FirstApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();

        ~FirstApp();

        FirstApp(const FirstApp &) = delete;

        FirstApp &operator=(const FirstApp &) = delete;

        void run();

    private:
        void loadGameObjects(VkDescriptorSetLayout globalLayoutSetLayout, VkRenderPass renderPass);
        void AddLights();
        void MoveLights(float time);

        Window window{WIDTH, HEIGHT, "Vulkan Tutorial"};
        Device device{window};
        Renderer renderer{window, device};

        // note: order of declarations matters
        std::unique_ptr<DescriptorPool> globalPool{};
//  GameObject::Map gameObjects;
//    GameObject skyBox{GameObject::createGameObject()};

        std::shared_ptr<DescriptorSetLayout> textureSetLayout; //to RenderPipeline
        AssetsSystem assetSystem{device};
        MaterialSystem materialSystem;

        Scene* scene;
    };
}  // namespace lwmeta
