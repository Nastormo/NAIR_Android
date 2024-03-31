#ifndef DEBUG_MESSENGER_H
#define DEBUG_MESSENGER_H

#include "vk_include.h"
#include <vector>

class DebugMessenger
{
public:
  DebugMessenger() = default;
  DebugMessenger(const DebugMessenger &another) = delete;
  DebugMessenger &operator=(const DebugMessenger &another) = delete;

  void initDebugReportCallback(VkInstance instance);
  void Clear();


  static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData);

private:
  VkInstance m_instance = VK_NULL_HANDLE;
  VkDebugReportCallbackEXT m_debugReportCallback = VK_NULL_HANDLE;
};


#endif// CBVH_AO_DEBUGMESSENGER_H