#include "ComputePass.h"

#include "vk_utils.h"
#include "vk_pipeline.h"

ComputePass::ComputePass(VkDevice device, VkCommandPool cmdPool, const VulkanSwapChain &swapchain)
  : m_device(device), m_cmdPool(cmdPool), m_extent(swapchain.GetExtent()),
  m_swapChainSize(swapchain.GetImageCount())
{
  VkDescriptorSetLayoutBinding dsLayoutBinding {};
  dsLayoutBinding.binding = 0;
  dsLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  dsLayoutBinding.descriptorCount = 1;
  dsLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

  VkDescriptorSetLayoutCreateInfo dsLayoutCreateInfo {};
  dsLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  dsLayoutCreateInfo.bindingCount = 1;
  dsLayoutCreateInfo.pBindings = &dsLayoutBinding;

  VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_device, &dsLayoutCreateInfo, nullptr, &m_dsLayout));

  VkDescriptorPoolSize descriptorPoolSize {};
  descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  descriptorPoolSize.descriptorCount = m_swapChainSize;

  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo {};
  descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolCreateInfo.maxSets = m_swapChainSize;
  descriptorPoolCreateInfo.poolSizeCount = 1;
  descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

  VK_CHECK_RESULT(vkCreateDescriptorPool(m_device, &descriptorPoolCreateInfo, nullptr, &m_dPool));

  std::vector<VkDescriptorSetLayout> layouts(m_swapChainSize, m_dsLayout);

  VkDescriptorSetAllocateInfo dsAllocateInfo {};
  dsAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  dsAllocateInfo.descriptorPool = m_dPool;
  dsAllocateInfo.descriptorSetCount = m_swapChainSize;
  dsAllocateInfo.pSetLayouts = layouts.data();

  m_descriptorSets.resize(m_swapChainSize);
  VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &dsAllocateInfo, m_descriptorSets.data()));

  vk_utils::ComputePipelineMaker maker;

  maker.LoadShader(m_device, COMPUTE_SHADER_PATH.c_str(), nullptr, "main");

  m_layout = maker.MakeLayout(m_device, { m_dsLayout }, 0U);

  m_pipeline = maker.MakePipeline(m_device);

  VkCommandBufferAllocateInfo allocateInfo {};
  allocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocateInfo.commandPool        = m_cmdPool;
  allocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandBufferCount = m_swapChainSize;

  m_cmdBufs.resize(m_swapChainSize);
  vkAllocateCommandBuffers(m_device, &allocateInfo, m_cmdBufs.data());
}

void ComputePass::setBindings(std::vector<SwapchainAttachment> &swapchainAttachments)
{
  std::vector<VkDescriptorImageInfo> descriptorImageInfo(m_swapChainSize);
  std::vector<VkWriteDescriptorSet> writeDescriptorSet(m_swapChainSize);

  for (auto i = 0U; i < m_swapChainSize; ++i) {
    descriptorImageInfo[i].sampler = VK_NULL_HANDLE;
    descriptorImageInfo[i].imageView = swapchainAttachments[i].view;
    descriptorImageInfo[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    writeDescriptorSet[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet[i].dstSet = m_descriptorSets[i];
    writeDescriptorSet[i].dstBinding = 0U;
    writeDescriptorSet[i].descriptorCount = 1;
    writeDescriptorSet[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeDescriptorSet[i].pImageInfo = &descriptorImageInfo[i];
    writeDescriptorSet[i].pBufferInfo = nullptr;
    writeDescriptorSet[i].pTexelBufferView = nullptr;
  }

  vkUpdateDescriptorSets(m_device, uint32_t(writeDescriptorSet.size()), writeDescriptorSet.data(), 0, NULL);

  VkImageSubresourceRange subresourceRange {};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0U;
  subresourceRange.levelCount = 1U;
  subresourceRange.baseArrayLayer = 0U;
  subresourceRange.layerCount = 1U;

  VkImageMemoryBarrier inputImageMemoryBarrier {};
  inputImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  inputImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  inputImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  inputImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  inputImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
  inputImageMemoryBarrier.image = VK_NULL_HANDLE;
  inputImageMemoryBarrier.subresourceRange = subresourceRange;

  VkImageMemoryBarrier outputImageMemoryBarrier {};
  outputImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  outputImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  outputImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  outputImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
  outputImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  outputImageMemoryBarrier.image = VK_NULL_HANDLE;
  outputImageMemoryBarrier.subresourceRange = subresourceRange;

  for (auto i = 0U; i < m_swapChainSize; ++i) {
    inputImageMemoryBarrier.image = swapchainAttachments[i].image;
    outputImageMemoryBarrier.image = swapchainAttachments[i].image;
    inputImageBarrier.push_back(inputImageMemoryBarrier);
    outputImageBarrier.push_back(outputImageMemoryBarrier);
  }
}

void ComputePass::recordCmdBuffers() {
  for (size_t i = 0; i < m_swapChainSize; ++i) {
    recordCmdBuffer(i);
  }
}

void ComputePass::recordCmdBuffer(size_t bufIndex) {
  auto cmd = m_cmdBufs.at(bufIndex);
  vkResetCommandBuffer(cmd, 0);

  VkCommandBufferBeginInfo beginInfo {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  VK_CHECK_RESULT(vkBeginCommandBuffer(cmd, &beginInfo));

  vkCmdPipelineBarrier(cmd,VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,0,
          0, nullptr,
          0, nullptr,
          1, &inputImageBarrier[bufIndex]);

  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline);
  vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_layout, 0, 1, &m_descriptorSets[bufIndex], 0, 0);
  vkCmdDispatch(cmd, 256, 256, 1);

  vkCmdPipelineBarrier(cmd,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT ,VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,0,
                       0, nullptr,
                       0, nullptr,
                       1, &outputImageBarrier[bufIndex]);

  VK_CHECK_RESULT(vkEndCommandBuffer(cmd));
}
