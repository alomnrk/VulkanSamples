#pragma once

#include "model.h"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>
#include <concepts>

namespace lwmeta {

    struct Component{
        virtual void kek(){

        };
    };

    struct TransformComponent : Component {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4();

        glm::mat4 rot4();

        glm::mat3 normalMatrix();

        void kek() override{

        }
    };

    struct PointLightComponent : Component {
        PointLightComponent(float lightIntensity, glm::vec3 color) : lightIntensity{lightIntensity}, color{color}{};
        float lightIntensity = 1.0f;
        glm::vec3 color{};

        void kek() override{

        }
    };

    struct PointLightRendererComponent : Component {
        PointLightRendererComponent(float radius, PointLightComponent *pointLight) : radius{radius}, pointLight{pointLight}{};
        float radius;
        PointLightComponent* pointLight;

        void kek() override{

        }
    };

    struct DirectLightComponent : Component {
        float lightIntensity = 1.0f;
        glm::vec3 color{};

        void kek() override{

        }
    };

    struct MaterialComponent : Component {
        MaterialComponent(uint32_t materialId) : materialId{materialId}{};
        uint32_t materialId;

        void kek() override{

        }
    };

    struct MeshComponent : Component {
        MeshComponent(uint32_t meshId) : meshId{meshId}{};
        uint32_t meshId;

        void kek() override{

        }
    };

    template<class T>
    concept DerivedComponent = std::is_base_of<Component, T>::value;

    class GameObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, GameObject>;

        static GameObject createGameObject() {
            static id_t currentId = 0;
            return GameObject{currentId++};
        }

        static GameObject* createGameObjectPtr() {
            static id_t currentId = 0;
            return new GameObject{currentId++};
        }

//  static GameObject makePointLight(
//      float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

        GameObject(const GameObject &) = delete;

        GameObject &operator=(const GameObject &) = delete;

        GameObject(GameObject &&) = default;

        GameObject &operator=(GameObject &&) = default;

        id_t getId() { return id; }


        template<DerivedComponent T>
        id_t AddComponent(T *component) {
            components.template emplace(current_component_id++, component);
            return current_component_id - 1;
        }

        template<DerivedComponent T>
        T* GetComponent(){
            for (auto &kv: components){
                if(T *ptr = dynamic_cast<T*>(kv.second))
                    return ptr;
            }
            return nullptr;
        }

//  glm::vec3 color{};
        TransformComponent transform{};

        // Optional pointer components
//        std::shared_ptr<Model> model{};
//        std::unique_ptr<PointLightComponent> pointLight = nullptr;

//        std::vector<uint32_t> materialsId;

        std::unordered_map<id_t, Component*> components;

    private:
        GameObject(id_t objId) : id{objId} {}

        id_t id;
        id_t current_component_id = 0;
    };
}  // namespace lwmeta
