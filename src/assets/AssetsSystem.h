//
// Created by Artem Ustinov on 27.09.2022.
//

#ifndef LWMETAVERSE_ASSETSSYSTEM_H
#define LWMETAVERSE_ASSETSSYSTEM_H

//#include "game_object.h"
#include "Texture.h"
#include "../vulkan_utils/descriptors.h"
#include "../vulkan_utils/CubeMap.h"
#include <iostream>
#include "model.h"

namespace lwmeta {

//    struct Material {
//    public:
//        Material(id_t materialId) {
//            id = materialId;
//        }
//
////    static Material* createMaterial(Texture* texture, DescriptorSetLayout* layout, DescriptorPool* pool) {
////        static id_t currentId = 0;
////        return new Material(currentId++, texture, layout, pool);
////    }
//
//        virtual id_t getId() { return id; }
//
//        VkDescriptorSet descriptorSet;
//
//    protected:
//        id_t id;
//    };
//
//    struct TextureMaterial : Material {
//    public:
//        TextureMaterial(id_t materialId, Texture *texture, uint32_t binding, DescriptorSetLayout *layout,
//                        DescriptorPool *pool) : Material(materialId) {
//            id = materialId;
//
//            VkDescriptorImageInfo imageInfo;
//            imageInfo.sampler = texture->getSampler();
//            imageInfo.imageView = texture->getImageView();
//            imageInfo.imageLayout = texture->getImageLayout();
//
//            std::cout << "create material" << std::endl;
//            DescriptorWriter(*layout, *pool)
//                    .writeImage(binding, &imageInfo)
//                    .build(descriptorSet);
//            std::cout << "finish create material" << std::endl;
//        }
//
//        static TextureMaterial *
//        createMaterial(Texture *texture, uint32_t binding, DescriptorSetLayout *layout, DescriptorPool *pool) {
//            static id_t currentId = 0;
//            return new TextureMaterial(currentId++, texture, binding, layout, pool);
//        }
//    };
//
//    struct SkyBoxMaterial : Material {
//        SkyBoxMaterial(id_t materialId, CubeMap *cubeMap, DescriptorSetLayout *layout, DescriptorPool *pool) : Material(
//                materialId) {
//            id = materialId;
//
//            VkDescriptorImageInfo imageInfo;
//            imageInfo.sampler = cubeMap->getSampler();
//            imageInfo.imageView = cubeMap->getImageView();
//            imageInfo.imageLayout = cubeMap->getImageLayout();
//
//            std::cout << "create material" << std::endl;
//            DescriptorWriter(*layout, *pool)
//                    .writeImage(0, &imageInfo)
//                    .build(descriptorSet);
//            std::cout << "finish create material" << std::endl;
//        }
//    };
//
//    struct BlackHoleMaterial : Material {
//        BlackHoleMaterial(id_t materialId, CubeMap *cubeMap, Texture *texture, DescriptorSetLayout *layout,
//                          DescriptorPool *pool) : Material(materialId) {
//            id = materialId;
//
//            VkDescriptorImageInfo imageInfo;
//            imageInfo.sampler = cubeMap->getSampler();
//            imageInfo.imageView = cubeMap->getImageView();
//            imageInfo.imageLayout = cubeMap->getImageLayout();
//
//            VkDescriptorImageInfo diskInfo;
//            diskInfo.sampler = texture->getSampler();
//            diskInfo.imageView = texture->getImageView();
//            diskInfo.imageLayout = texture->getImageLayout();
//
//            std::cout << "create material" << std::endl;
//            DescriptorWriter(*layout, *pool)
//                    .writeImage(0, &imageInfo)
//                    .writeImage(1, &diskInfo)
//                    .build(descriptorSet);
//            std::cout << "finish create material" << std::endl;
//        }
//
//        static BlackHoleMaterial *
//        createMaterial(CubeMap *cubeMap, Texture *texture, DescriptorSetLayout *layout, DescriptorPool *pool) {
//            static id_t currentId = 0;
//            return new BlackHoleMaterial(currentId++, cubeMap, texture, layout, pool);
//        }
//    };
//
//
//    struct LitMaterial : Material {
//        LitMaterial(id_t materialId, Texture *texture, DescriptorSetLayout *layout,
//                          DescriptorPool *pool) : Material(materialId) {
//            id = materialId;
//
//            VkDescriptorImageInfo imageInfo;
//            imageInfo.sampler = texture->getSampler();
//            imageInfo.imageView = texture->getImageView();
//            imageInfo.imageLayout = texture->getImageLayout();
//
//            std::cout << "create material" << std::endl;
//            DescriptorWriter(*layout, *pool)
//                    .writeImage(0, &imageInfo)
//                    .build(descriptorSet);
//            std::cout << "finish create material" << std::endl;
//        }
//
//
//    };
//
//    struct DissolveLitMaterial : Material {
//        struct DissolveLitMaterialUbo{
//            float strength;
//        };
//
//        DissolveLitMaterial(Device &device, id_t materialId, Texture *texture, DescriptorSetLayout *layout,
//                    DescriptorPool *pool) : Material(materialId) {
//            id = materialId;
//
//
//            VkDescriptorImageInfo imageInfo;
//            imageInfo.sampler = texture->getSampler();
//            imageInfo.imageView = texture->getImageView();
//            imageInfo.imageLayout = texture->getImageLayout();
//
//
//            uboBuffer = std::make_shared<Buffer>(
//                    device,
//                    sizeof(DissolveLitMaterialUbo),
//                    1,
//                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
//            uboBuffer->map();
//
//
//            std::cout << "create material" << std::endl;
//            auto bufferInfo = uboBuffer->descriptorInfo();
//            DescriptorWriter(*layout, *pool)
//                    .writeImage(0, &imageInfo)
//                    .writeBuffer(1, &bufferInfo)
//                    .build(descriptorSet);
//
//            std::cout << "finish create material" << std::endl;
//        }
//
//        void UpdateValues(float strength){
//            DissolveLitMaterialUbo ubo{};
//            ubo.strength = strength;
//
//            uboBuffer->writeToBuffer(&ubo);
//            uboBuffer->flush();
//        }
//
//    private:
//        std::shared_ptr<Buffer> uboBuffer;
//    };


    class AssetsSystem {
    public:
        using id_t = unsigned int;


        AssetsSystem(Device &device);

        ~AssetsSystem();

        AssetsSystem(const AssetsSystem &) = delete;

        AssetsSystem &operator=(const AssetsSystem &) = delete;

        id_t AddTexture(const std::string filepath, uint32_t desired_channels, bool gammaCorrection);

        id_t AddCubeMap(const std::vector<char *> &fileNames);

        id_t AddModel(Model *model);

        Model* GetModel(id_t modelId);

        Texture* GetTexture(id_t textureId){
            return textures.at(textureId);
        }

//        id_t
//        CreateTextureMaterial(id_t texture_id, uint32_t binding, DescriptorSetLayout *layout, DescriptorPool *pool);
//
//        id_t CreateCubeMapMaterial(id_t cubeMap_id, DescriptorSetLayout *layout, DescriptorPool *pool);
//
//        id_t
//        CreateBlackHoleMaterial(id_t cubeMap_id, id_t disktexture, DescriptorSetLayout *layout, DescriptorPool *pool);

//        id_t
//        CreateLitMaterial(id_t texture, DescriptorSetLayout *layout, DescriptorPool *pool);

        id_t
        CreateDissolveLitMaterial(id_t texture, DescriptorSetLayout *layout, DescriptorPool *pool);


//        Material *GetMaterial(id_t material_id);

    private:
        Device &device;
        uint32_t currentId = 0;

        std::unordered_map<id_t, Texture*> textures;
//        std::unordered_map<id_t, Material*> materials;
        std::unordered_map<id_t, CubeMap*> cubeMaps;
        std::unordered_map<id_t, Model*> models;
    };
}


#endif //LWMETAVERSE_ASSETSSYSTEM_H
