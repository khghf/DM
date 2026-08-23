#include<Core/RHI/Backend/Vulkan/VulkanPipeline.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<Core/RHI/Backend/Vulkan/VulkanRenderPass.h>
#include<Core/RHI/Backend/Vulkan/VulkanShader.h>
#include<Core/RHI/Backend/Vulkan/VulkanShaderProgram.h>
#include<Core/RHI/Backend/Vulkan/VulkanTypeMapping.h>
#include<Core/RHI/Backend/Vulkan/VulkanTexture.h>
#include<stdexcept>
#include<set>
namespace DM::RHI
{
	
	VulkanPipeline::VulkanPipeline(VulkanDevice* device, const RHIPipelineDesc& desc)
		: m_Device(device)
	{
		VulkanRenderPass* renderPass = static_cast<VulkanRenderPass*>(desc.RenderPass);
		assert(renderPass && "Forgot to setup renderPass in RHIPipelineDesc");

		//********************输入装配：图元怎么连********************//
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = ToVkTopology(desc.Topology); // topology：图元拓扑类型
		inputAssembly.primitiveRestartEnable = VK_FALSE;//图元重启,实现不连续绘制(也可使用退化三角形实现，缓存会更大但性能更好)


		//********************视口+裁剪********************//
		//这里将帧缓冲、纹理坐标原点从左上角设置为右下角，翻转NDC的y轴
		//如果是动态参数(在下面我将其设置成了动态参数)则需要在每次调用设置视口命令前更改所以这里就不需要初始其值
		VkExtent2D	extent{};
		VkViewport	viewport{};
		VkRect2D	scissor{};
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		//********************光栅化配置********************
		//包含：图形填充、面剔除、多重采样
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		// depthClamp：深度钳位
		// VK_TRUE = 将深度值钳位到 [0,1] 范围
		// VK_FALSE = 允许深度值超出范围(会被丢弃)
		// 需要启用扩展才能设为 VK_TRUE
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE; 
		rasterizer.polygonMode =ToVkPolygonMode(desc.FillMode); 
		rasterizer.cullMode =	ToVkCullMode(desc.CullMode);
		// frontFace：正面定义
		// VK_FRONT_FACE_CLOCKWISE = 顺时针顶点顺序为正面
		// VK_FRONT_FACE_COUNTER_CLOCKWISE = 逆时针为正面
		// 与 cullMode 配合使用，决定哪些三角形被剔除
		//注意：在后面视口被设置了动态参数，每帧都需要调用vkcmdSetViewport并且我把vulkan的NDC坐标的y轴进行了反转
		//		所以原来的正面从顺时针变成来逆时针，背面从逆时帧变成了顺时针
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		// lineWidth：线宽(仅在 polygonMode = LINE 时有效)
		// 支持宽线需要扩展，大多数硬件只支持 1.0
		rasterizer.lineWidth = 1.0f;

		//********************配置采样、深度、模板********************//
		VkPipelineMultisampleStateCreateInfo multisample{};
		multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample.rasterizationSamples = renderPass->GetvkSampleCount();
		if (renderPass->GetvkSampleCount() != VK_SAMPLE_COUNT_1_BIT)
		{
			multisample.sampleShadingEnable = VK_TRUE;
			multisample.minSampleShading = 0.2f;
		}

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = desc.DepthTest ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = desc.DepthWrite ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = ToVkCompareOp(desc.DepthCompare);
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		//********************配置颜色混合********************//
		VkPipelineColorBlendAttachmentState colorBlendAtt{};
		// 控制哪些颜色通道可以写入帧缓冲
		colorBlendAtt.colorWriteMask =VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAtt.blendEnable = VK_FALSE;

		std::vector<VkPipelineColorBlendAttachmentState>attachments{ colorBlendAtt };

		VkPipelineColorBlendStateCreateInfo colorBlend{};
		colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		
		colorBlend.logicOpEnable = VK_FALSE;
		colorBlend.logicOp = VK_LOGIC_OP_COPY;
		colorBlend.attachmentCount = attachments.size();
		colorBlend.pAttachments = attachments.data();
		colorBlend.blendConstants[0] = 0.0f;
		colorBlend.blendConstants[1] = 0.0f;
		colorBlend.blendConstants[2] = 0.0f;
		colorBlend.blendConstants[3] = 0.0f;
		

		//********************配置动态参数********************//
		std::vector<VkDynamicState> dynamicStates = 
		{
			VK_DYNAMIC_STATE_VIEWPORT,   // 视口参数动态
			VK_DYNAMIC_STATE_SCISSOR     // 剪刀参数动态
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = (uint32_t)(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();


		VulkanShaderProgram* shaderProgram = static_cast<VulkanShaderProgram*>(desc.ShaderProgram);


		VkGraphicsPipelineCreateInfo pipelineCI{};
		pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCI.stageCount = shaderProgram->m_vkPipelineShaderStages.size();
		pipelineCI.pStages = shaderProgram->m_vkPipelineShaderStages.data();
		pipelineCI.pVertexInputState = &shaderProgram->m_vkVertexInput;
		pipelineCI.pInputAssemblyState = &inputAssembly;
		pipelineCI.pViewportState = &viewportState;
		pipelineCI.pRasterizationState = &rasterizer;
		pipelineCI.pMultisampleState = &multisample;
		pipelineCI.pDepthStencilState = &depthStencil;
		pipelineCI.pColorBlendState = &colorBlend;
		pipelineCI.pDynamicState =&dynamicState;
		pipelineCI.layout = shaderProgram->m_vkPipelineLayout;
		pipelineCI.renderPass = renderPass->GetvkRenderPass();
		pipelineCI.subpass = 0;
		pipelineCI.basePipelineHandle = VK_NULL_HANDLE;  
		pipelineCI.basePipelineIndex = -1;              
		VkResult r = vkCreateGraphicsPipelines(m_Device->GetvkDevice(), VK_NULL_HANDLE,1, &pipelineCI, nullptr, &m_Pipeline);
		if (r != VK_SUCCESS)throw std::runtime_error("failed to create pipline");
			
	}

	VulkanPipeline::~VulkanPipeline()
	{
		
		vkDestroyPipeline(m_Device->GetvkDevice(), m_Pipeline, nullptr);
	
	}
	
} // namespace DM::RHI
