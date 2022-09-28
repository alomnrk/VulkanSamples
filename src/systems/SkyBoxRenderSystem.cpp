//
// Created by Artem Ustinov on 26.09.2022.
//

#include "SkyBoxRenderSystem.h"
#include "../model.h"

namespace lwmeta {

    struct SkyBoxPushConstants {
        glm::mat4 modelMatrix{1.f};
    };


    void SkyBoxRenderSystem::createModel(){
        Model::Builder modelBuilder{};
        modelBuilder.vertices =
                {
                        // left face (white)
                        {{-1, -1, -1}, {.9f, .9f, .9f}},
                        {{-1, 1, 1}, {.9f, .9f, .9f}},
                        {{-1, -1, 1}, {.9f, .9f, .9f}},
                        {{-1, 1, -1}, {.9f, .9f, .9f}},

                        // right face (yellow)
                        {{1, -1, -1}, {.8f, .8f, .1f}},
                        {{1, 1, 1}, {.8f, .8f, .1f}},
                        {{1, -1, 1}, {.8f, .8f, .1f}},
                        {{1, 1, -1}, {.8f, .8f, .1f}},

                        // top face (orange, remember y axis points down)
                        {{-1, -1, -1}, {.9f, .6f, .1f}},
                        {{1, -1, 1}, {.9f, .6f, .1f}},
                        {{-1, -1, 1}, {.9f, .6f, .1f}},
                        {{1, -1, -1}, {.9f, .6f, .1f}},

                        // bottom face (red)
                        {{-1, 1, -1}, {.8f, .1f, .1f}},
                        {{1, 1, 1}, {.8f, .1f, .1f}},
                        {{-1, 1, 1}, {.8f, .1f, .1f}},
                        {{1, 1, -1}, {.8f, .1f, .1f}},

                        // nose face (blue)
                        {{-1, -1, 1}, {.1f, .1f, .8f}},
                        {{1, 1, 1}, {.1f, .1f, .8f}},
                        {{-1, 1, 1}, {.1f, .1f, .8f}},
                        {{1, -1, 1}, {.1f, .1f, .8f}},

                        // tail face (green)
                        {{-1, -1, -1}, {.1f, .8f, .1f}},
                        {{1, 1, -1}, {.1f, .8f, .1f}},
                        {{-1, 1, -1}, {.1f, .8f, .1f}},
                        {{1, -1, -1}, {.1f, .8f, .1f}},
                };
        modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  6,  5,  4,  5,  7,  8,  10,  9, 8, 9, 11,
                                12, 13, 14, 12, 15, 13, 16, 18, 17, 19, 16, 17, 20, 21, 22, 20, 23, 21};

        skyBoxModel = std::make_unique<Model>(device, modelBuilder);
    }



    SkyBoxRenderSystem::SkyBoxRenderSystem(
            Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
            : device{device} {
        createModel();
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    void SkyBoxRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SkyBoxPushConstants);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

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

    void SkyBoxRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::SkyBoxPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
                device,
                "shaders/skybox.vert.spv",
                "shaders/skybox.frag.spv",
                pipelineConfig);
    }


    void SkyBoxRenderSystem::renderGameObjects(FrameInfo& frameInfo) {
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
        push.modelMatrix = frameInfo.skyBox.transform.rot4();

        vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SkyBoxPushConstants),
                &push);


        skyBoxModel->bind(frameInfo.commandBuffer);
        skyBoxModel->draw(frameInfo.commandBuffer);
    }

    SkyBoxRenderSystem::~SkyBoxRenderSystem() {
        //todo
    }
}