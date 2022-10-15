//
// Created by Artem Ustinov on 09.10.2022.
//

#ifndef LWMETAVERSE_MATERIALINSTANCE_H
#define LWMETAVERSE_MATERIALINSTANCE_H

#include <vulkan/vulkan.h>
#include "../utils/frame_info.h"
#include "../vulkan_utils/descriptors.h"
#include "../assets/Texture.h"
#include "../vulkan_utils/buffer.h"

namespace lwmeta {
    struct BasePushConstantData {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };


    struct MaterialInstance {
    public:
        MaterialInstance(uint32_t id, std::string materialName, VkPipelineLayout &pipelineLayout) : _id{id}, materialName{materialName}, pipelineLayout{pipelineLayout} {};

        virtual id_t getId() { return _id; }

        virtual void bind(FrameInfo &frameInfo){};
//        VkDescriptorSet getMaterialSet(){return materialSet;}
    protected:
        std::string materialName;
        uint32_t _id;
        VkPipelineLayout &pipelineLayout;
    };

    struct DissolveLitMaterialInstance : MaterialInstance {
    public:
        DissolveLitMaterialInstance(uint32_t id, DescriptorSetLayout &layout, Texture *texture, DescriptorPool *pool);

        void UpdateStrength(float strength) {
            DissolveLitMaterialUbo ubo{};
            ubo.strength = strength;

            uboBuffer->writeToBuffer(&ubo);
            uboBuffer->flush();
        }

    private:
        struct DissolveLitMaterialUbo {
            float strength;
        };

        std::unique_ptr<Buffer> uboBuffer;
        VkDescriptorSet materialSet;
    };
}


#endif //LWMETAVERSE_MATERIALINSTANCE_H
