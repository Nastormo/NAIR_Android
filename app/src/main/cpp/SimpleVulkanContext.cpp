#include "SimpleVulkanContext.h"

#include "vk_utils.h"

void SimpleVulkanContext::CreateInstance(const std::vector<const char*> &instanceExtensions,
  const std::vector<const char*> &validationLayers) {
  m_instanceExtensions = instanceExtensions;
  m_validationLayers = validationLayers;

  VkApplicationInfo appInfo = {};
  appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pNext              = nullptr;
  appInfo.pApplicationName   = "MsuCource";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
  appInfo.pEngineName        = "SimpleEngine";
  appInfo.engineVersion      = VK_MAKE_VERSION(0, 1, 0);
  appInfo.apiVersion         = VK_MAKE_VERSION(1, 1, 0);

#ifdef NDEBUG
  bool enableValidation = false;
#else
  bool enableValidation = false;
#endif

  VK_CHECK_RESULT(volkInitialize());

  m_instance = vk_utils::createInstance(enableValidation, m_validationLayers, m_instanceExtensions, &appInfo);

  if (enableValidation) {
    m_debugMessenger.initDebugReportCallback(m_instance);
  }

  volkLoadInstance(m_instance);
}

void SimpleVulkanContext::CreateDevice(uint32_t a_deviceId, const std::vector<const char*> &deviceExtensions,
  VkPhysicalDeviceFeatures deviceFeatures) {
  m_deviceExtensions = deviceExtensions;
  m_enabledDeviceFeatures = deviceFeatures;

  m_physicalDevice = vk_utils::findPhysicalDevice(m_instance, true, a_deviceId, m_deviceExtensions);

  m_device = vk_utils::createLogicalDevice(m_physicalDevice, m_validationLayers, m_deviceExtensions,
    m_enabledDeviceFeatures, m_queueFamilyIDXs, VK_QUEUE_COMPUTE_BIT);

  volkLoadDevice(m_device);

  vkGetDeviceQueue(m_device, m_queueFamilyIDXs.graphics, 0, &m_graphicsQueue);
  vkGetDeviceQueue(m_device, m_queueFamilyIDXs.transfer, 0, &m_transferQueue);
}

void SimpleVulkanContext::Clear() {
  if(m_device != VK_NULL_HANDLE)
  {
    vkDestroyDevice(m_device, nullptr);
    m_device = VK_NULL_HANDLE;
  }

  m_debugMessenger.Clear();

  if(m_instance != VK_NULL_HANDLE) {
    vkDestroyInstance(m_instance, nullptr);
    m_instance = VK_NULL_HANDLE;
  }
}
