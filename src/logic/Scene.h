//
// Created by Artem Ustinov on 02.10.2022.
//

#ifndef LWMETAVERSE_SCENE_H
#define LWMETAVERSE_SCENE_H

#include "game_object.h"
#include "../utils/camera.h"
#include "LightSystem.h"

namespace lwmeta {
    class Scene {
    public:
        Scene(GameObject *skyBox) : skyBox{skyBox}{}

        void AddLitObject(GameObject gameObject);
        void AddPointLight(GameObject gameObject);


//        Camera camera;

        GameObject::Map litObjects;
        GameObject::Map pointLights;
        GameObject *skyBox;

        std::unique_ptr<GameObject> player;

        LightSystem lightSystem;
    };
}

#endif //LWMETAVERSE_SCENE_H
