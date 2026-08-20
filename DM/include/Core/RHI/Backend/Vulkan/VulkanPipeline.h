#pragma once
#include"Core/RHI/RHIPipeline.h"
#include<Core/RHI/Backend/Vulkan/Buffer/VulkanUniformBuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<vulkan/vulkan.h>
#include"VulkanShaderProgram.h"
namespace DM::RHI
{
	class VulkanDevice;
	class VulkanSwapchain;
	/// <summary>
	/// VkPipeline：
	///			预先烘培好的图形渲染管线，包含管线中的每个阶段(顶点着色器->细分着色器->几何着色器->光栅化阶段->片段着色器->颜色混合阶段)
	///	需要设置：
	///	1、各阶段着色器
	/// 2、顶点输入布局
	/// 3、图元类型
	/// 4、视口(需要交换链中的VkExtent2D)、裁剪
	/// 5、光栅化阶段：深度钳位、填充模式、面剔除
	/// 6、设置采样
	///	7、深度/模板：是否启用深度测试及模板测试、深度比较丢弃设置
	/// 8、颜色混合：颜色混合设置(当前片段颜色 + 帧缓冲已有颜色 -> 最终颜色)
	/// 9、管线布局：待补充
	/// 10、子通道(可选)：
	/// </summary>
	class VulkanPipeline : public RHIPipeline
	{
	public:
		VulkanPipeline(VulkanDevice* device, const RHIPipelineDesc& desc);
		~VulkanPipeline() override;

		VkPipeline       GetPipeline() const { return m_Pipeline; }
		VkPipelineLayout GetLayout()   const { return m_ShaderProgram->m_vkPipelineLayout; }
		VulkanShaderProgram* GetShaderProgram()const { return m_ShaderProgram; }

	private:
		VulkanDevice*     m_Device;  
		VkPipeline        m_Pipeline; 

		VulkanShaderProgram* m_ShaderProgram;
	};
} // namespace DM::RHI
