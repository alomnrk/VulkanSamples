cmake_minimum_required(VERSION 3.23)
project(LWMetaverse)

set(CMAKE_CXX_STANDARD 20)


add_executable(LWMetaverse src/main.cpp src/vulkan_utils/window.h src/vulkan_utils/window.cpp src/utils/utils.h src/vulkan_utils/swap_chain.h src/vulkan_utils/swap_chain.cpp
        src/utils/renderer.h src/utils/renderer.cpp src/vulkan_utils/pipeline.h src/vulkan_utils/pipeline.cpp src/assets/model.h src/assets/model.cpp
        src/logic/game_object.h src/logic/game_object.cpp src/utils/frame_info.h src/vulkan_utils/device.h src/vulkan_utils/device.cpp
        src/vulkan_utils/descriptors.h src/vulkan_utils/descriptors.cpp src/utils/camera.h src/utils/camera.cpp src/vulkan_utils/buffer.h src/vulkan_utils/buffer.cpp
        src/utils/keyboard_movement_controller.cpp src/utils/keyboard_movement_controller.h src/logic/first_app.h src/logic/first_app.cpp
        src/systems/point_light_system.h src/systems/point_light_system.cpp src/systems/LitRenderSystem.h src/systems/LitRenderSystem.cpp src/assets/Texture.cpp src/assets/Texture.h src/vulkan_utils/CubeMap.cpp src/vulkan_utils/CubeMap.h src/systems/SkyBoxRenderSystem.cpp src/systems/SkyBoxRenderSystem.h src/assets/AssetsSystem.cpp src/assets/AssetsSystem.h src/systems/BlackHoleRenderSystem.cpp src/systems/TransfusionRenderSystem.h src/systems/TransfusionRenderSystem.cpp src/systems/BlackHoleRenderSystem.h src/systems/DissolveRenderSystem.h src/systems/DissolveRenderSystem.cpp src/logic/Scene.cpp src/logic/Scene.h src/marerials/Material.h src/systems/UniversalRenderSystem.h src/systems/UniversalRenderSystem.cpp src/logic/LightSystem.cpp src/logic/LightSystem.h src/marerials/MaterialInstance.h src/marerials/MaterialSystem.cpp src/marerials/MaterialSystem.h src/marerials/LitMaterialInstance.h src/marerials/PBRMaterialInstance.h)

#Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(LWMetaverse Vulkan::Vulkan)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/glfw-master/)
target_link_libraries(LWMetaverse glfw)

target_include_directories(LWMetaverse PUBLIC external/tinyobjloader)



find_program(GLSL_VALIDATOR glslangValidator HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
        )

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/shaders/*.frag"
        "${PROJECT_SOURCE_DIR}/shaders/*.vert"
        )

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)