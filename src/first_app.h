#pragma once

#include "descriptors.h"
#include "device.h"
#include "game_object.h"
#include "renderer.h"
#include "window.h"
#include "AssetsSystem.h"
#include "Scene.h"

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
  void loadGameObjects();

  Window window{WIDTH, HEIGHT, "Vulkan Tutorial"};
  Device device{window};
  Renderer renderer{window, device};

  // note: order of declarations matters
  std::unique_ptr<DescriptorPool> globalPool{};
//  GameObject::Map gameObjects;
//    GameObject skyBox{GameObject::createGameObject()};

  std::unique_ptr<DescriptorSetLayout> textureSetLayout; //to RenderPipeline
  AssetsSystem assetSystem{device};

  Scene* scene;
};
}  // namespace lwmeta
