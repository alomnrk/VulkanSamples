//
// Created by Artem Ustinov on 25.09.2022.
//

#ifndef LWMETAVERSE_TEXTURE_H
#define LWMETAVERSE_TEXTURE_H

#include "device.h"
#include <string.h>
#include <vulkan/vulkan.h>

namespace lwmeta{
    class Texture {
    public:
        Texture(Device &device, const std::string &filepath);
        ~Texture();

        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;
        Texture(Texture &&) = delete;
        Texture &operator=(Texture &&) = delete;

        VkSampler getSampler() { return sampler; }
        VkImageView getImageView() { return imageView; }
        VkImageLayout getImageLayout() { return imageLayout; }
    private:
        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
        void generateMipmaps();

        int width, height, mipLevels;

        Device& device;
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        VkSampler sampler;
        VkFormat imageFormat;
        VkImageLayout imageLayout;
    };
    };

#endif //LWMETAVERSE_TEXTURE_H
