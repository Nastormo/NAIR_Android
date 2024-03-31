#ifndef SIMPLE_VULKAN_CONTEXT_H
#define SIMPLE_VULKAN_CONTEXT_H

#include <vector>

#include "vk_include.h"
#include "vk_utils.h"
#include "DebugMessenger.h"

class SimpleVulkanContext
{
public:
  SimpleVulkanContext() = default;

  [[nodiscard]] VkInstance                  GetInstance()        const { return m_instance; }
  [[nodiscard]] VkPhysicalDevice            GetPhysDevice()      const { return m_physicalDevice; }
  [[nodiscard]] VkDevice                    GetDevice()          const { return m_device; }
  [[nodiscard]] const vk_utils::QueueFID_T &GetQueueFamilyIDXs() const { return m_queueFamilyIDXs; }
  [[nodiscard]] VkQueue                     GetGraphicsQueue()   const { return m_graphicsQueue; }
  [[nodiscard]] VkQueue                     GetTransferQueue()   const { return m_transferQueue; }


  void CreateInstance(const std::vector<const char*> &instanceExtensions, const std::vector<const char*> &validationLayers);
  void CreateDevice(uint32_t a_deviceId, const std::vector<const char*> &deviceExtensions, VkPhysicalDeviceFeatures deviceFeatures);

  void Clear();

private:
  VkInstance m_instance = VK_NULL_HANDLE;
  std::vector<const char*> m_instanceExtensions;
  std::vector<const char*> m_validationLayers;

  DebugMessenger m_debugMessenger;

  VkPhysicalDeviceFeatures m_enabledDeviceFeatures = {};
  std::vector<const char*> m_deviceExtensions      = {};

  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkDevice         m_device         = VK_NULL_HANDLE;
  VkQueue          m_graphicsQueue  = VK_NULL_HANDLE;
  VkQueue          m_transferQueue  = VK_NULL_HANDLE;

  vk_utils::QueueFID_T m_queueFamilyIDXs {UINT32_MAX, UINT32_MAX, UINT32_MAX};
};

#endif // SIMPLE_VULKAN_CONTEXT_H