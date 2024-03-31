cmake_minimum_required(VERSION 3.22.1)

project(course)

add_compile_definitions(USE_VOLK)


add_library(${CMAKE_PROJECT_NAME} SHARED
        App.cpp
        SimpleRender.cpp
        SimpleVulkanContext.cpp
        ComputePass.cpp
        DebugMessenger.cpp
)

add_subdirectory(third_party)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        third_party/volk
        third_party/vkutils
)

target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        android
        log
        vkutils
        volk
)