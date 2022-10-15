//
// Created by Artem Ustinov on 02.10.2022.
//

#include "Scene.h"

namespace lwmeta{

    void Scene::AddLitObject(GameObject gameObject) {
        litObjects.emplace(gameObject.getId(), std::move(gameObject));
    }

    void Scene::AddPointLight(GameObject gameObject) {
        pointLights.emplace(gameObject.getId(), std::move(gameObject));
    }
}