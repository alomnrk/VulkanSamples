//
// Created by Artem Ustinov on 26.09.2022.
//

#ifndef LWMETAVERSE_CUBEMAP_H
#define LWMETAVERSE_CUBEMAP_H


#include <vector>
#include <vulkan/vulkan.h>

#include "device.h"

namespace lwmeta {

    class CubeMap {
    public:
        using id_t = unsigned int;

        CubeMap(Device &device, const std::vector<char *> &fileNames, id_t cubeMapId);

        ~CubeMap();

        VkSampler getSampler() { return sampler; }

        VkImageView getImageView() { return imageView; }

        VkImageLayout getImageLayout() { return imageLayout; }

        id_t getId() { return id; }

        static CubeMap *createTexture(Device &device, const std::vector<char *> &fileNames) {
            static id_t currentId = 0;
            return new CubeMap(device, fileNames, currentId++);
        }

    private:
        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

        static constexpr int NUMBER_OF_CUBEMAP_IMAGES = 6;

        id_t id;

        Device &device;
        int width, height, mipLevels;
        VkImage image;
        VkDeviceMemory imageMemory;
        VkSampler sampler;
        VkImageLayout imageLayout; //?
        VkImageView imageView;
    };
};

#endif //LWMETAVERSE_CUBEMAP_H
