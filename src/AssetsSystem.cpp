//
// Created by Artem Ustinov on 27.09.2022.
//

#include "AssetsSystem.h"

namespace lwmeta{
    AssetsSystem::AssetsSystem(Device &device) : device{device} {}

    id_t AssetsSystem::AddTexture(const std::string filepath) {
        Texture* texture = Texture::createTexture(device, filepath);
        textures.emplace(texture->getId(), texture);
        return texture->getId();
    }

    id_t AssetsSystem::CreateMaterial(uint32_t texture_id, DescriptorSetLayout* layout, DescriptorPool* pool) {
        auto material = Material::createMaterial(textures.at(texture_id), layout, pool);
        materials.emplace(material->getId(), material);
        return material->getId();
    }

    Material *AssetsSystem::GetMaterial(lwmeta::AssetsSystem::id_t material_id) {
        return materials.at(material_id);
    }

    AssetsSystem::~AssetsSystem() {
        for (auto& kv : materials) {
            auto &material = kv.second;
            delete material;
        }

        for (auto& kv : textures) {
            auto &texture = kv.second;
            delete texture;
        }
    }
}