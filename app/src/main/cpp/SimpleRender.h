#ifndef SIMPLE_RENDER_H
#define SIMPLE_RENDER_H

#include <android/asset_manager_jni.h>

#include "vk_swapchain.h"

#include "SimpleVulkanContext.h"
#include "ComputePass.h"

class SimpleRender final
{
public:
  SimpleRender() = default;
  SimpleRender(const SimpleRender &another) = delete;
  SimpleRender &operator=(const SimpleRender &another) = delete;

  ~SimpleRender() { Cleanup(); };

  [[nodiscard]] inline uint32_t     GetWidth()         const { return m_width;    }
  [[nodiscard]] inline uint32_t     GetHeight()        const { return m_height;   }
  [[nodiscard]] inline VkInstance   GetVkInstance()    const { return m_context.GetInstance(); }

  void InitVulkan(const std::vector<const char*> &a_requiredExtensions, uint32_t a_deviceId);
  void InitPresentation(VkSurfaceKHR a_surface);

  void DrawFrame();
  void Synchronize() { vkDeviceWaitIdle(m_context.GetDevice()); };

private:
  uint32_t m_width = 1280U;
  uint32_t m_height = 720U;
  uint32_t m_framesInFlight  = 2U;
  bool m_vsync = false;

  SimpleVulkanContext m_context {};

  VkCommandPool m_commandPool = VK_NULL_HANDLE;

  VkSurfaceKHR m_surface = VK_NULL_HANDLE;
  VulkanSwapChain m_swapchain {};

  struct
  {
    uint32_t currentFrame = 0U;
    VkQueue queue = VK_NULL_HANDLE;
    std::vector<VkSemaphore> imageAvailable;
  } m_presentationResources;

  std::unique_ptr<ComputePass>  m_computePass;
  std::vector<VkFence> m_renderFinished;


  void CreateSwapChainDependentResources();


  void Cleanup();
};


#endif //SIMPLE_RENDER_H