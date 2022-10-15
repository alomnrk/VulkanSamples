//
// Created by Artem Ustinov on 29.09.2022.
//

#include "BlackHoleRenderSystem.h"

namespace lwmeta{
    struct SkyBoxPushConstants {
        glm::mat4 modelMatrix{1.f};
    };


    void BlackHoleRenderSystem::createModel(){
        Model::Builder modelBuilder{};
        modelBuilder.vertices =
                {
                        // left face (white)
                        {{-1, -1, -1}, {.9f, .9f, .9f},},
                        {{-1, 1, 1}, {.9f, .9f, .9f},},
                        {{-1, -1, 1}, {.9f, .9f, .9f}},
                        {{-1, 1, -1}, {.9f, .9f, .9f}},

                        // right face (yellow)
                        {{1, -1, -1}, {.8f, .8f, .1f}, {0, 0, 0}, {0, 0}},
                        {{1, 1, 1}, {.8f, .8f, .1f}, {0, 0, 0}, {0, 0}},
                        {{1, -1, 1}, {.8f, .8f, .1f}, {0, 0, 0}, {0, 0}},
                        {{1, 1, -1}, {.8f, .8f, .1f}, {0, 0, 0}, {0, 0}},

                        // top face (orange, remember y axis points down)
                        {{-1, -1, -1}, {.9f, .6f, .1f}, {0, 0, 0}, {0, 0}},
                        {{1, -1, 1}, {.9f, .6f, .1f}, {0, 0, 0}, {0, 0}},
                        {{-1, -1, 1}, {.9f, .6f, .1f}, {0, 0, 0}, {0, 0}},
                        {{1, -1, -1}, {.9f, .6f, .1f}, {0, 0, 0}, {0, 0}},

                        // bottom face (red)
                        {{-1, 1, -1}, {.8f, .1f, .1f}, {0, 0, 0}, {0, 0}},
                        {{1, 1, 1}, {.8f, .1f, .1f}, {0, 0, 0}, {0, 0}},
                        {{-1, 1, 1}, {.8f, .1f, .1f}, {0, 0, 0}, {0, 0}},
                        {{1, 1, -1}, {.8f, .1f, .1f}, {0, 0, 0}, {0, 0}},

                        // nose face (blue)
                        {{-1, -1, 1}, {.1f, .1f, .8f}, {0, 0, 0}, {0, 0}},
                        {{1, 1, 1}, {.1f, .1f, .8f}, {0, 0, 0}, {0, 0}},
                        {{-1, 1, 1}, {.1f, .1f, .8f}, {0, 0, 0}, {0, 0}},
                        {{1, -1, 1}, {.1f, .1f, .8f}, {0, 0, 0}, {0, 0}},

                        // tail face (green)
                        {{-1, -1, -1}, {.1f, .8f, .1f}, {0, 0, 0}, {0, 0}},
                        {{1, 1, -1}, {.1f, .8f, .1f}, {0, 0, 0}, {0, 0}},
                        {{-1, 1, -1}, {.1f, .8f, .1f}, {0, 0, 0}, {0, 0}},
                        {{1, -1, -1}, {.1f, .8f, .1f}, {0, 0, 0}, {0, 0}},
                };
        modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  6,  5,  4,  5,  7,  8,  10,  9, 8, 9, 11,
                                12, 13, 14, 12, 15, 13, 16, 18, 17, 19, 16, 17, 20, 21, 22, 20, 23, 21};

        skyBoxModel = std::make_unique<Model>(device, modelBuilder);
    }



    BlackHoleRenderSystem::BlackHoleRenderSystem(
            Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout textureSetLayout, AssetsSystem& assetsSystem)
            : device{device}, assetsSystem{assetsSystem} {
        createModel();
        createPipelineLayout(globalSetLayout, textureSetLayout);
        createPipeline(renderPass);
    }

    void BlackHoleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout cubeMapSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SkyBoxPushConstants);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout, cubeMapSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void BlackHoleRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::SkyBoxPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
                device,
                "shaders/blackhole.vert.spv",
                "shaders/blackhole.frag.spv",
                pipelineConfig);
    }


    void BlackHoleRenderSystem::renderGameObjects(FrameInfo& frameInfo, GameObject &skybox) {
        pipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                0,
                1,
                &frameInfo.globalDescriptorSet,
                0,
                nullptr);


        SkyBoxPushConstants push{};
        push.modelMatrix = skybox.transform->rot4();

        vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SkyBoxPushConstants),
                &push);


        VkDescriptorSet b;
        vkCmdBindDescriptorSets(
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                1,
                1,
                &b,
                0,
                nullptr);

        skyBoxModel->bind(frameInfo.commandBuffer);
        skyBoxModel->draw(frameInfo.commandBuffer);
    }

    BlackHoleRenderSystem::~BlackHoleRenderSystem() {
        //todo
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }
}