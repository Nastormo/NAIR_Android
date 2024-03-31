set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_ANDROID_KHR)

include_directories(volk)
add_subdirectory(volk)

include_directories(vkutils)
add_library(vkutils
    STATIC
    vkutils/vk_copy.cpp
    vkutils/vk_utils.cpp
    vkutils/vk_buffers.cpp
    vkutils/vk_descriptor_sets.cpp
    vkutils/vk_images.cpp
    vkutils/vk_swapchain.cpp
    vkutils/vk_pipeline.cpp
    vkutils/geom/vk_mesh.cpp
    vkutils/geom/cmesh.cpp
    vkutils/vk_quad.cpp
    vkutils/vk_fbuf_attachment.cpp
)
target_link_libraries(vkutils PRIVATE volk)
target_compile_features(vkutils PRIVATE cxx_std_17)