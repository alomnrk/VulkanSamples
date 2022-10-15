//
// Created by Artem Ustinov on 09.10.2022.
//

#ifndef LWMETAVERSE_LIGHTSYSTEM_H
#define LWMETAVERSE_LIGHTSYSTEM_H

#include <vector>
#include <glm/glm.hpp>
//#include "game_object.h"

namespace lwmeta {
    struct PointLight {
        glm::vec4 position{};  // ignore w
        glm::vec4 color{};     // w is intensity
    };

    struct DirectionalLight {
        glm::vec4 direction{};  // ignore w
        glm::vec4 color{};     // w is intensity
    };

    class LightSystem {
    public:
        LightSystem() {}

        void AddPointLight(PointLight *l){
            pointLights.push_back(l);
        }

        void AddDirectionalLight(DirectionalLight *l) {
            directionalLights.push_back(l);
        }

        void SetAmbient(glm::vec4 ambientLight){
            this->ambientLight = ambientLight;
        }

        std::vector<PointLight*> pointLights;
        std::vector<DirectionalLight*> directionalLights;
        glm::vec4 ambientLight;
    };
}

#endif //LWMETAVERSE_LIGHTSYSTEM_H
