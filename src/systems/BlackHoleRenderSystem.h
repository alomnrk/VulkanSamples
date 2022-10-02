//
// Created by Artem Ustinov on 29.09.2022.
//

#ifndef LWMETAVERSE_BLACKHOLERENDERSYSTEM_H
#define LWMETAVERSE_BLACKHOLERENDERSYSTEM_H

#include "../device.h"
#include "../frame_info.h"
#include "../pipeline.h"
#include "../model.h"
#include "../AssetsSystem.h"

namespace lwmeta {
    class BlackHoleRenderSystem {
    public:
        BlackHoleRenderSystem(
                Device
        &device,
        VkRenderPass renderPass, VkDescriptorSetLayout
        globalSetLayout, VkDescriptorSetLayout textureSetLayout, AssetsSystem& assetsSystem);

        ~BlackHoleRenderSystem();

        BlackHoleRenderSystem(const BlackHoleRenderSystem &) = delete;

        BlackHoleRenderSystem &operator=(const BlackHoleRenderSystem &) = delete;

        void renderGameObjects(FrameInfo &frameInfo);

    private:
        AssetsSystem &assetsSystem;

        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout cubeMapSetLayout);

        void createPipeline(VkRenderPass renderPass);

        void createModel();

        Device &device;

        std::unique_ptr <Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;

        std::unique_ptr<Model> skyBoxModel;
    };
}

#endif //LWMETAVERSE_BLACKHOLERENDERSYSTEM_H
