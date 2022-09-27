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
        CubeMap(Device &device, const std::vector<char*> &fileNames);
        ~CubeMap();

    private:
        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

        static constexpr int NUMBER_OF_CUBEMAP_IMAGES = 6;

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
