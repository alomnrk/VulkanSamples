//
// Created by Artem Ustinov on 26.09.2022.
//

#include "CubeMap.h"
#include "../external/stb/stb_image.h"
#include "buffer.h"


namespace lwmeta {

    CubeMap::CubeMap(Device &device, const std::vector<char *> &fileNames) : device{device} {
        int prevWidth = -1;
        int prevHeight = -1;
        int bytesPerPixel;

        std::vector<stbi_uc*> files;

        for (int i = 0; i < NUMBER_OF_CUBEMAP_IMAGES; i++) {
            files.push_back(stbi_load(fileNames[i], &width, &height, &bytesPerPixel, 4));
            if (prevWidth != -1){
                if (prevWidth != width){
                    throw std::runtime_error("failed to create cube map!");
                }
            }

            prevWidth = width;
            prevHeight = height;
        }

        VkDeviceSize layerSize = width * height;
        VkDeviceSize imageSize = layerSize * NUMBER_OF_CUBEMAP_IMAGES;

        VkDeviceMemory stagingBufferMemory;

        //staging buffer
        Buffer stagingBuffer{
                device,
                4,
                static_cast<uint32_t>(imageSize),
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };


        stagingBuffer.map();
        for (int i = 0; i < NUMBER_OF_CUBEMAP_IMAGES; i++) {
            stagingBuffer.writeToBuffer(files[i], layerSize, layerSize * i);
        }

        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = format;
        imageInfo.mipLevels = 0; //?
        imageInfo.arrayLayers = NUMBER_OF_CUBEMAP_IMAGES;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;


        device.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory);
        transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        device.copyBufferToImage(stagingBuffer.getBuffer(), image, static_cast<u_int32_t>(width), static_cast<u_int32_t>(height), NUMBER_OF_CUBEMAP_IMAGES);

//        TransitionImageLayout
//                (
//                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
//                );

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        samplerInfo.maxAnisotropy = 4.0;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        vkCreateSampler(device.device(), &samplerInfo, nullptr, &sampler);

        VkImageViewCreateInfo imageViewInfo {};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        imageViewInfo.format = format;
        imageViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;
        imageViewInfo.subresourceRange.levelCount = mipLevels;
        imageViewInfo.image = image;

        vkCreateImageView(device.device(), &imageViewInfo, nullptr, &imageView);

        for (int i = 0; i < NUMBER_OF_CUBEMAP_IMAGES; i++) {
            stbi_image_free(files[i]);
        }
    }

    CubeMap::~CubeMap() {
        vkDestroyImage(device.device(), image, nullptr);
        vkFreeMemory(device.device(), imageMemory, nullptr);
        vkDestroyImageView(device.device(), imageView, nullptr);
        vkDestroySampler(device.device(), sampler, nullptr);
    }


    void CubeMap::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else {
            throw std::runtime_error("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        device.endSingleTimeCommands(commandBuffer);
    }

};