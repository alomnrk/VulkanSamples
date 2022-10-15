//
// Created by Artem Ustinov on 09.10.2022.
//

#include "MaterialSystem.h"

#include "LitMaterialInstance.h"
#include "PBRMaterialInstance.h"

namespace lwmeta{
    uint32_t MaterialSystem::CreateLitMaterialInstance(Texture *texture, DescriptorPool *pool) {
        auto material = materials.at("Lit").get();
        auto materialSetLayout = material->getMaterialSetLayout();
        return materials.at("Lit")->AddInstance(new LitMaterialInstance(0, *materialSetLayout, texture, pool, material->pipelineLayout));
    }

    uint32_t MaterialSystem::CreatePBRMaterialInstance(Texture *albedoTexture, Texture* normalTexture, Texture *metallicTexture, Texture *roughnessTexture, DescriptorPool *pool) {
        auto material = materials.at("PBR").get();
        auto materialSetLayout = material->getMaterialSetLayout();
        return materials.at("PBR")->AddInstance(new PBRMaterialInstance(0, *materialSetLayout, albedoTexture, normalTexture, metallicTexture, roughnessTexture, pool, material->pipelineLayout));
    }


    void MaterialSystem::CreatePBRMaterial(lwmeta::Device &device, VkDescriptorSetLayout globalSetLayout,
                                           VkRenderPass renderPass) {
        std::string vertPath = "shaders/lit.vert.spv";
        std::string fragPath = "shaders/pbr.frag.spv";
        auto materialSetLayout = DescriptorSetLayout::Builder(device)
                .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; //?
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(BasePushConstantData);

        materials.emplace("PBR",
                          std::make_unique<Material>(device, vertPath, fragPath, globalSetLayout, materialSetLayout,
                                                     pushConstantRange, renderPass));
    }

    void MaterialSystem::CreateLitMaterial(lwmeta::Device &device, VkDescriptorSetLayout globalSetLayout,
                                           VkRenderPass renderPass) {
        std::string vertPath = "shaders/lit.vert.spv";
        std::string fragPath = "shaders/lit.frag.spv";
        auto materialSetLayout = DescriptorSetLayout::Builder(device)
                .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
//                .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; //?
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(BasePushConstantData);

        materials.emplace("Lit",
                          std::make_unique<Material>(device, vertPath, fragPath, globalSetLayout, materialSetLayout,
                                                     pushConstantRange, renderPass));
    }

//    void MaterialSystem::CreateDissolveLitMaterial(lwmeta::Device &device, std::string vertPath, std::string fragPath,
//                                                   VkDescriptorSetLayout globalSetLayout, VkRenderPass renderPass) {
//        auto materialSetLayout = DescriptorSetLayout::Builder(device)
//                .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
//                .addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
//                .build();
//
//        VkPushConstantRange pushConstantRange{};
//        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; //?
//        pushConstantRange.offset = 0;
//        pushConstantRange.size = sizeof(BasePushConstantData);
//
//        materials.emplace("DissolveLit",
//                          std::make_unique<Material>(device, vertPath, fragPath, globalSetLayout, materialSetLayout,
//                                                     pushConstantRange, renderPass));
//    }
}