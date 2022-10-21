//
// Created by Artem Ustinov on 09.10.2022.
//

#pragma once

#ifndef LWMETAVERSE_MATERIALSYSTEM_H
#define LWMETAVERSE_MATERIALSYSTEM_H

#include <vulkan/vulkan.h>
#include "../assets/Texture.h"
#include "Material.h"

namespace lwmeta {
    class MaterialSystem {
    public:
        MaterialSystem() {};

        void CreateLitMaterial(lwmeta::Device &device,
                               VkDescriptorSetLayout globalSetLayout, VkRenderPass renderPass);

        void CreatePBRMaterial(lwmeta::Device &device,
                               VkDescriptorSetLayout globalSetLayout, VkRenderPass renderPass);

        void CreateFractalMaterial(lwmeta::Device &device,
                               VkDescriptorSetLayout globalSetLayout, VkRenderPass renderPass);


//        void CreateDissolveLitMaterial(lwmeta::Device &device, std::string vertPath, std::string fragPath,
//                                       VkDescriptorSetLayout globalSetLayout, VkRenderPass renderPass);

        uint32_t CreateLitMaterialInstance(Texture *texture, DescriptorPool *pool);
        uint32_t CreatePBRMaterialInstance(Texture *albedoTexture, Texture* normalTexture, Texture *metallicTexture, Texture *roughnessTexture, DescriptorPool *pool);
        uint32_t CreateFractalMaterialInstance(DescriptorPool *pool);


        MaterialInstance *GetMaterialInstance(std::string name, uint32_t instance_id) {
            return materials.at(name)->GetMaterialInstance(instance_id);
        };

        Material *GetMaterial(std::string name) {
            return materials.at(name).get();
        };
    private:
        std::unordered_map <std::string, std::unique_ptr<Material>> materials;
    };
}

#endif //LWMETAVERSE_MATERIALSYSTEM_H
