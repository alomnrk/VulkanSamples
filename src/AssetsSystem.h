//
// Created by Artem Ustinov on 27.09.2022.
//

#ifndef LWMETAVERSE_ASSETSSYSTEM_H
#define LWMETAVERSE_ASSETSSYSTEM_H

#include "game_object.h"
#include "Texture.h"
#include "descriptors.h"
#include "CubeMap.h"
#include <iostream>

namespace lwmeta{

struct Material{
public:
    Material(id_t materialId){
        id = materialId;
    }

//    static Material* createMaterial(Texture* texture, DescriptorSetLayout* layout, DescriptorPool* pool) {
//        static id_t currentId = 0;
//        return new Material(currentId++, texture, layout, pool);
//    }

    id_t getId() { return id; }

    VkDescriptorSet descriptorSet;

protected:
    id_t id;
};

    struct TextureMaterial : Material{
    public:
        TextureMaterial(id_t materialId, Texture* texture, DescriptorSetLayout* layout, DescriptorPool* pool) : Material(materialId){
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

        static TextureMaterial* createMaterial(Texture* texture, DescriptorSetLayout* layout, DescriptorPool* pool) {
            static id_t currentId = 0;
            return new TextureMaterial(currentId++, texture, layout, pool);
        }
    };

    struct SkyBoxMaterial : Material{
    SkyBoxMaterial(id_t materialId, CubeMap* cubeMap, DescriptorSetLayout* layout, DescriptorPool* pool) : Material(materialId){
        id = materialId;

        VkDescriptorImageInfo imageInfo;
        imageInfo.sampler = cubeMap->getSampler();
        imageInfo.imageView = cubeMap->getImageView();
        imageInfo.imageLayout = cubeMap->getImageLayout();

        std::cout << "create material" << std::endl;
        DescriptorWriter(*layout, *pool)
                .writeImage(0, &imageInfo)
                .build(descriptorSet);
        std::cout << "finish create material" << std::endl;
    }

    static SkyBoxMaterial* createMaterial(CubeMap* cubeMap, DescriptorSetLayout* layout, DescriptorPool* pool) {
        static id_t currentId = 0;
        return new SkyBoxMaterial(currentId++, cubeMap, layout, pool);
    }
};


class AssetsSystem {
public:
    using id_t = unsigned int;

    AssetsSystem(Device &device);
    ~AssetsSystem();

    AssetsSystem(const AssetsSystem &) = delete;
    AssetsSystem &operator=(const AssetsSystem &) = delete;

    id_t AddTexture(const std::string filepath);
    id_t AddCubeMap(const std::vector<char*> &fileNames);

    id_t CreateTextureMaterial(id_t texture_id, DescriptorSetLayout* layout, DescriptorPool* pool);
    id_t CreateCubeMapMaterial(id_t cubeMap_id, DescriptorSetLayout* layout, DescriptorPool* pool);


    Material* GetMaterial(id_t material_id);

private:
    Device &device;

    std::unordered_map<id_t, Texture*> textures;
    std::unordered_map<id_t, Material*> materials;
    std::unordered_map<id_t, CubeMap*> cubeMaps;
};
}

#endif //LWMETAVERSE_ASSETSSYSTEM_H
