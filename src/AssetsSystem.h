//
// Created by Artem Ustinov on 27.09.2022.
//

#ifndef LWMETAVERSE_ASSETSSYSTEM_H
#define LWMETAVERSE_ASSETSSYSTEM_H

#include "game_object.h"
#include "Texture.h"
#include "descriptors.h"
#include <iostream>

namespace lwmeta{

struct Material{
    Material(id_t materialId, Texture* texture, DescriptorSetLayout* layout, DescriptorPool* pool){

        id = materialId;

        VkDescriptorImageInfo imageInfo;
        imageInfo.sampler = texture->getSampler();
        imageInfo.imageView = texture->getImageView();
        imageInfo.imageLayout = texture->getImageLayout();

        std::cout << "create material" << std::endl;
        DescriptorWriter(*layout, *pool)
                .writeImage(0, &imageInfo)
                .build(descriptorSet);
        std::cout << "finish create material" << std::endl;
    }

    static Material* createMaterial(Texture* texture, DescriptorSetLayout* layout, DescriptorPool* pool) {
        static id_t currentId = 0;
        return new Material(currentId++, texture, layout, pool);
    }

    id_t getId() { return id; }

    VkDescriptorSet descriptorSet;

private:
    id_t id;
};

class AssetsSystem {
public:
    using id_t = unsigned int;

    AssetsSystem(Device &device);
    ~AssetsSystem();

    AssetsSystem(const AssetsSystem &) = delete;
    AssetsSystem &operator=(const AssetsSystem &) = delete;

    id_t AddTexture(const std::string filepath);
    id_t CreateMaterial(id_t texture_id, DescriptorSetLayout* layout, DescriptorPool* pool);

    Material* GetMaterial(id_t material_id);

private:
    Device &device;

    std::unordered_map<id_t, Texture*> textures;
    std::unordered_map<id_t, Material*> materials;
};
}

#endif //LWMETAVERSE_ASSETSSYSTEM_H
