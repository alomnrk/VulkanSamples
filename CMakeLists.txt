cmake_minimum_required(VERSION 3.23)
project(LWMetaverse)

set(CMAKE_CXX_STANDARD 20)


add_executable(LWMetaverse src/main.cpp src/window.h src/window.cpp src/utils.h src/swap_chain.h src/swap_chain.cpp
        src/renderer.h src/renderer.cpp src/pipeline.h src/pipeline.cpp src/model.h src/model.cpp
        src/game_object.h src/game_object.cpp src/frame_info.h src/device.h src/device.cpp
        src/descriptors.h src/descriptors.cpp src/camera.h src/camera.cpp src/buffer.h src/buffer.cpp
        src/keyboard_movement_controller.cpp src/keyboard_movement_controller.h src/first_app.h src/first_app.cpp
        src/systems/point_light_system.h src/systems/point_light_system.cpp src/systems/simple_render_system.h src/systems/simple_render_system.cpp src/Texture.cpp src/Texture.h src/CubeMap.cpp src/CubeMap.h src/systems/SkyBoxRenderSystem.cpp src/systems/SkyBoxRenderSystem.h src/AssetsSystem.cpp src/AssetsSystem.h src/systems/BlackHoleRenderSystem.cpp src/systems/BlackHoleRenderSystem.h)

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