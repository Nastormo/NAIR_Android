#ifndef COMPUTE_PASS_H
#define COMPUTE_PASS_H

#include "volk.h"
#include "vk_utils.h"
#include "vk_swapchain.h"

class ComputePass
{
public:
  ComputePass(VkDevice device, VkCommandPool cmdPool, const VulkanSwapChain &swapchain);
  ComputePass(const ComputePass &another) = delete;
  ComputePass &operator=(const ComputePass &another) = delete;
  ~ComputePass() = default;

  void setBindings(std::vector<SwapchainAttachment> &swapchainAttachments);
  void createPipeline();
  void recordCmdBuffers();

  [[nodiscard]] VkCommandBuffer getCmdBuf(size_t imgIndex) const { return m_cmdBufs.at(imgIndex); }
private:
  const std::string COMPUTE_SHADER_PATH = "shaders/shader.comp.spv";

  VkDevice m_device;
  VkCommandPool m_cmdPool;

  VkExtent2D m_extent;
  uint32_t m_swapChainSize;
  VkDescriptorSet m_descriptorSet;
  VkPipeline m_pipeline;

  VkDescriptorSetLayout m_dsLayout;
  VkPipelineLayout m_layout;


  VkDescriptorPool m_dPool;
  std::vector<VkDescriptorSet> m_descriptorSets;

  std::vector<VkCommandBuffer> m_cmdBufs;

  std::vector<VkImageMemoryBarrier> inputImageBarrier;
  std::vector<VkImageMemoryBarrier> outputImageBarrier;

  void recordCmdBuffer(size_t bufIndex);
};


#endif// COMPUTE_PASS_H