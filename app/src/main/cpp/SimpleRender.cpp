#include "SimpleRender.h"

#include "vk_utils.h"

static std::vector<const char*> GetRequiredValidationLayers()
{
  std::vector<const char*> validationLayers {
    "VK_LAYER_KHRONOS_validation",
    "VK_LAYER_LUNARG_monitor"
  };

  return validationLayers;
}

static std::vector<const char*> GetRequiredDeviceExtensions()
{
  std::vector<const char*> deviceExtensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };

  return deviceExtensions;
}

static VkPhysicalDeviceFeatures GetRequiredDeviceFeatures()
{
  VkPhysicalDeviceFeatures deviceFeatures {};

  return deviceFeatures;
}

void SimpleRender::InitVulkan(const std::vector<const char*> &a_requiredExtensions, uint32_t a_deviceId)
{
  std::vector<const char*> instanceExtensions = a_requiredExtensions;

  m_context.CreateInstance(instanceExtensions, GetRequiredValidationLayers());

  auto deviceExtensions = GetRequiredDeviceExtensions();
  auto deviceFeatures = GetRequiredDeviceFeatures();
  m_context.CreateDevice(a_deviceId, deviceExtensions, deviceFeatures);

  m_commandPool = vk_utils::createCommandPool(m_context.GetDevice(), m_context.GetQueueFamilyIDXs().graphics,
    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

void SimpleRender::InitPresentation(VkSurfaceKHR a_surface)
{
  m_surface = a_surface;
  m_presentationResources.currentFrame = 0;

  CreateSwapChainDependentResources();
}

void SimpleRender::CreateSwapChainDependentResources()
{
  m_presentationResources.queue = m_swapchain.CreateSwapChain(m_context.GetPhysDevice(), m_context.GetDevice(),
    m_surface, m_width, m_height, m_framesInFlight, m_vsync);

  VkSemaphoreCreateInfo semaphoreInfo {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  m_presentationResources.imageAvailable.resize(m_framesInFlight);
  m_renderFinished.resize(m_framesInFlight);

  for (uint32_t i = 0; i < m_framesInFlight; ++i) {
    VK_CHECK_RESULT(vkCreateSemaphore(m_context.GetDevice(), &semaphoreInfo, nullptr,
      &m_presentationResources.imageAvailable[i]));
    VK_CHECK_RESULT(vkCreateFence(m_context.GetDevice(), &fenceInfo, nullptr, &m_renderFinished[i]));
  }

  m_computePass = std::make_unique<ComputePass>(m_context.GetDevice(), m_commandPool, m_swapchain);
  std::vector<SwapchainAttachment> swapchainAttachments;
  for (auto i = 0U; i < m_swapchain.GetImageCount(); ++i) {
    swapchainAttachments.push_back(m_swapchain.GetAttachment(i));
  }
  m_computePass->setBindings(swapchainAttachments);
  m_computePass->recordCmdBuffers();
}

void SimpleRender::Cleanup()
{
  if (!m_presentationResources.imageAvailable.empty()) {
    for (auto &semaphore : m_presentationResources.imageAvailable) {
      vkDestroySemaphore(m_context.GetDevice(), semaphore, nullptr);
    }
    m_presentationResources.imageAvailable.clear();
  }

  if (!m_renderFinished.empty()) {
    for (auto &fence : m_renderFinished) {
      vkDestroyFence(m_context.GetDevice(), fence, nullptr);
    }
    m_renderFinished.clear();
  }

  m_swapchain.Cleanup();

  if(m_surface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(m_context.GetInstance(), m_surface, nullptr);
    m_surface = VK_NULL_HANDLE;
  }

  if (m_commandPool != VK_NULL_HANDLE) {
    vkDestroyCommandPool(m_context.GetDevice(), m_commandPool, nullptr);
    m_commandPool = VK_NULL_HANDLE;
  }

  m_context.Clear();
}

void SimpleRender::DrawFrame()
{
  vkWaitForFences(m_context.GetDevice(), 1, &m_renderFinished[m_presentationResources.currentFrame], VK_TRUE, UINT64_MAX);
  VK_CHECK_RESULT(vkResetFences(m_context.GetDevice(), 1, &m_renderFinished[m_presentationResources.currentFrame]))

  uint32_t imageIdx;
  auto result = m_swapchain.AcquireNextImage(m_presentationResources.imageAvailable[m_presentationResources.currentFrame], &imageIdx);
  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
  {
    RUN_TIME_ERROR("Failed to acquire the next swapchain image!");
  }

  VkSemaphore waitSemaphores[] = {m_presentationResources.imageAvailable[m_presentationResources.currentFrame]};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT};

  std::vector<VkCommandBuffer> submitCmdBufs;
  submitCmdBufs.push_back(m_computePass->getCmdBuf(imageIdx));

  auto cmdBuff = m_computePass->getCmdBuf(imageIdx);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1U;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1U;
  submitInfo.pCommandBuffers = &cmdBuff;
  submitInfo.signalSemaphoreCount = 0U;
  submitInfo.pSignalSemaphores = nullptr;

  VK_CHECK_RESULT(vkQueueSubmit(m_context.GetGraphicsQueue(), 1, &submitInfo, m_renderFinished[m_presentationResources.currentFrame]));

  VkResult presentRes = m_swapchain.QueuePresent(m_presentationResources.queue, imageIdx,
                                                 VK_NULL_HANDLE);

  if (presentRes != VK_SUCCESS)
  {
    RUN_TIME_ERROR("Failed to present swapchain image");
  }

  m_presentationResources.currentFrame = (m_presentationResources.currentFrame + 1) % m_framesInFlight;
  Synchronize();
}
