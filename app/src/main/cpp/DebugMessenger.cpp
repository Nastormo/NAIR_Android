#include "DebugMessenger.h"
#include "vk_utils.h"

VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::debugReportCallbackFn(VkDebugReportFlagsEXT flags,
  VkDebugReportObjectTypeEXT /* objectType */, uint64_t /* object */, size_t /* location */, int32_t /* messageCode */,
  const char* pLayerPrefix, const char* pMessage, void* /* pUserData */)
{
  std::stringstream ss{};
  ss << "[VALIDATION MSG] " << pLayerPrefix << ": " << pMessage << std::endl;

  if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
    VK_UTILS_LOG_ERROR(ss.str());
  } else if ((flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) || (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)) {
    VK_UTILS_LOG_WARNING(ss.str());
  } else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
    VK_UTILS_LOG_INFO(ss.str());
  } else {
    VK_UTILS_LOG_DEBUG(ss.str());
  }

  return VK_FALSE;
}

void DebugMessenger::initDebugReportCallback(VkInstance instance)
{
  m_instance = instance;
  vk_utils::initDebugReportCallback(m_instance, &debugReportCallbackFn, &m_debugReportCallback);
}

void DebugMessenger::Clear()
{
  if(m_debugReportCallback != VK_NULL_HANDLE)
  {
    vkDestroyDebugReportCallbackEXT(m_instance, m_debugReportCallback, nullptr);
    m_debugReportCallback = VK_NULL_HANDLE;
    m_instance = VK_NULL_HANDLE;
  }
}
