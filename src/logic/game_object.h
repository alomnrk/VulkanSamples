#pragma once

#include "../utils/frame_info.h"
#include "../assets/model.h"
#include "../assets/AssetsSystem.h"
#include "../marerials/MaterialSystem.h"
#include "LightSystem.h"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>
#include <concepts>
#include <iostream>

namespace lwmeta {

    struct Component{
        virtual int getId(){
            //todo
            return 0;
        };

        virtual void Update(){};
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
    };

    struct PointLightComponent : Component {
        PointLightComponent(LightSystem *lightSystem, float lightIntensity, glm::vec3 color, TransformComponent* transformComponent) {
            this->transformComponent = transformComponent;
            pointLight = {glm::vec4(transformComponent->translation, 1), glm::vec4(color, lightIntensity)};
            lightSystem->AddPointLight(&pointLight);
        };

        void Update() override{
            pointLight.position = glm::vec4(transformComponent->translation, 1);
//            std::cout << transformComponent->translation.x << std::endl;
        }

//        float lightIntensity = 1.0f;
//        glm::vec3 color{};

        PointLight pointLight;
        TransformComponent* transformComponent;
    };

    struct PointLightRendererComponent : Component {
        PointLightRendererComponent(float radius, PointLightComponent *pointLight) : radius{radius}, pointLight{pointLight}{};
        float radius;
        PointLightComponent* pointLight;
    };

    struct DirectionalLightComponent : Component {
        DirectionalLightComponent(LightSystem *lightSystem, float lightIntensity, glm::vec3 color, TransformComponent* transformComponent) {
            this->transformComponent = transformComponent;
            directionalLight = {transformComponent->rot4() * glm::vec4(0, 0, 1, 1), glm::vec4(color, lightIntensity)};
            lightSystem->AddDirectionalLight(&directionalLight);
        };

        void Update() override{
            directionalLight.direction = transformComponent->rot4() * glm::vec4(0, 0, 1, 1);
//            std::cout << transformComponent->translation.x << std::endl;
        }

        DirectionalLight directionalLight;
        TransformComponent* transformComponent;
    };

    struct MaterialComponent : Component {
        MaterialComponent(MaterialSystem* materialSystem, std::string materialName, uint32_t materialId) : materialSystem{materialSystem}, materialName{materialName}, materialId{materialId}{};

        void bindMaterialGlobal(FrameInfo &frameInfo){
            materialSystem->GetMaterial(materialName)->bind(frameInfo);
        };

        void bindInstance(FrameInfo &frameInfo){
            materialSystem->GetMaterialInstance(materialName, materialId)->bind(frameInfo);
        };

        MaterialInstance* getMaterialInstance(){
            return materialSystem->GetMaterialInstance(materialName, materialId);
        }

        MaterialSystem* materialSystem;
        std::string materialName;
        uint32_t materialId;
    };

    struct MeshComponent : Component {
        MeshComponent(AssetsSystem* assetSystem, uint32_t meshId) : assetSystem{assetSystem}, meshId{meshId}{};

        Model* GetModel(){
            return assetSystem->GetModel(meshId);
        }

        uint32_t meshId;
        AssetsSystem* assetSystem;
    };

    struct UsualRendererComponent : Component {
        UsualRendererComponent(uint32_t rendererId, MeshComponent* meshComponent, MaterialComponent* materialComponent) : rendererId{rendererId}, meshComponent{meshComponent}, materialComponent{materialComponent}{};
        uint32_t rendererId;
        MeshComponent* meshComponent;
        MaterialComponent* materialComponent;

        void Render(FrameInfo &frameInfo){
            materialComponent->bindMaterialGlobal(frameInfo);
            materialComponent->bindInstance(frameInfo);

            auto model = meshComponent->GetModel();
            model->bind(frameInfo.commandBuffer);
            model->draw(frameInfo.commandBuffer);
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

        void Update(){
            for (auto c : components){
                c.second->Update();
            }
        }

        void Render(FrameInfo &frameInfo){
//            GetComponent<MaterialComponent>()->bind(materialSystem, frameInfo);
            GetComponent<UsualRendererComponent>()->Render(frameInfo);
        };

        std::unique_ptr<TransformComponent> transform;

        std::unordered_map<id_t, Component*> components;

    private:
        GameObject(id_t objId) : id{objId} {
            transform = std::make_unique<TransformComponent>();
        }

        id_t id;
        id_t current_component_id = 0;

//        AssetsSystem *assetSystem;
        MaterialSystem *materialSystem;
    };
}  // namespace lwmeta
