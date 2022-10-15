//
// Created by Artem Ustinov on 25.09.2022.
//

#ifndef LWMETAVERSE_TEXTURE_H
#define LWMETAVERSE_TEXTURE_H

#include "../vulkan_utils/device.h"
#include <string.h>
#include <vulkan/vulkan.h>
#include <unordered_map>

namespace lwmeta{
    class Texture {
    public:
        using id_t = unsigned int;
//        using Map = std::unordered_map<id_t, Texture>;

        Texture(Device &device, const std::string &filepath, id_t textureId, VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
        ~Texture();

        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;
        Texture(Texture &&) = delete;
        Texture &operator=(Texture &&) = delete;

        VkSampler getSampler() { return sampler; }
        VkImageView getImageView() { return imageView; }
        VkImageLayout getImageLayout() { return imageLayout; }

        id_t getId() { return id; }

        static Texture* createTexture(Device &device, const std::string &filepath, VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT) {
            static id_t currentId = 0;
            return new Texture(device, filepath, currentId++, samplerAddressMode);
        }
    private:


        id_t id;

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
