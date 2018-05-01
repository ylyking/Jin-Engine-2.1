#pragma once

#include "../Core/Vulkan.h"
#include "../Actor/Camera.h"
#include "../Actor/Object.h"
#include "../Actor/Light.h"
#include "../Core/Sky.h"

#include "PostProcess.h"
#include "../UI/GUI.h"

static Time timer;

enum RenderPassID
{
	GBUFFER = 0, MAIN
};

class Renderer
{
public:

	Renderer() :vulkanApp(NULL), layerCount(1), directionalLightUniformBuffer(NULL), directionalLightUniformMemory(NULL), pointLightUniformBuffer(NULL), pointLightUniformMemory(NULL),
		perFrameBuffer(NULL), perFrameBufferMemory(NULL)// , screenSpaceNoise(NULL)
	{
		/*
		if (screenSpaceNoise == NULL)
		{
			srand(time(NULL));
			screenSpaceNoise = new glm::vec4[MAX_SCREEN_WIDTH * MAX_SCREEN_HEIGHT];

			for (int i = 0; i < MAX_SCREEN_WIDTH * MAX_SCREEN_HEIGHT; i++)
			{
				screenSpaceNoise[i] = glm::vec4((double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX);
			}
		}
		*/
		
	}

	~Renderer()
	{
		//delete[] screenSpaceNoise;
	}

	void initialize(Vulkan* pVulkanApp);
	void reInitializeRenderer();

	void createSemaphore(VkSemaphore &semaphore)
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(vulkanApp->getDevice(), &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create semaphores!");
		}
	}

	void createSemaphores()
	{
		createSemaphore(gbufferSemaphore);
		createSemaphore(pbrSemaphore);
		createSemaphore(guiSemaphore);
		createSemaphore(presentSemaphore);
	}

	void deleteSemaphores()
	{
		vkDestroySemaphore(vulkanApp->getDevice(), gbufferSemaphore, nullptr);
		vkDestroySemaphore(vulkanApp->getDevice(), pbrSemaphore, nullptr);

		vkDestroySemaphore(vulkanApp->getDevice(), guiSemaphore, nullptr);

		vkDestroySemaphore(vulkanApp->getDevice(), presentSemaphore, nullptr);
	}


	void createQueues()
	{
		QueueFamilyIndices indices = vulkanApp->findQueueFamilies(vulkanApp->getPhysicalDevice(), vulkanApp->getSurface());
		
		
		vkGetDeviceQueue(vulkanApp->getDevice(), indices.computeFamily, 0, &frustumQueue);
		vkGetDeviceQueue(vulkanApp->getDevice(), indices.graphicsFamily, 0, &gbufferQueue);
		vkGetDeviceQueue(vulkanApp->getDevice(), indices.graphicsFamily, 0, &pbrQueue);

		vkGetDeviceQueue(vulkanApp->getDevice(), indices.graphicsFamily, 0, &guiQueue);		

		vkGetDeviceQueue(vulkanApp->getDevice(), indices.presentFamily, 0, &presentQueue);		
	}
	

	void createGbuffers();
	void updateGbuffers();
	void releaseGbuffers();
	void deleteGbuffers();

	void createSingleTriangularVertexBuffer();


	void createGbufferRenderPass();
	void createGbufferFramebuffers();
	void createGbufferCommandPool();
	void createGbufferCommandBuffers();
	void recordGbufferCommandBuffers();
	
	void createFrustumCullingCommandPool();
	void createFrustumCullingCommandBuffers();
	void recordFrustumCullingCommandBuffers(int groupSizeX, int groupSizeY, int groupSizeZ);

	void createSwapChain();
	void createSwapChainImageViews();

	void createDepthResources();
	void releaseDepthResources();
	void shutdownDepthResources();

	void createSSRDepthResources();
	void releaseSSRDepthResources();
	void shutdownSSRDepthResources();

	/*
	void createGUICanvas();
	void releaseGUICanvas();
	void deleteGUICanvas();

	void createGUIFrameBuffers();

	void createGUICommandPool()
	{
		vulkanApp->createCommandPool(guiCmdPool);
	}

	void createGUICommandBuffers()
	{
		vulkanApp->createCommandBuffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, guiFramebuffers, guiCmd, guiCmdPool);
	}

	void recordGUICommandBuffers()
	{
		std::vector<VkClearValue> clearValues;
		clearValues.resize(1);
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
		//clearValues[1].depthStencil = { 1.0f, 0 };

		vulkanApp->recordCommandBuffers(&guiCmd, guiCmdPool, &guiFramebuffers, "NEEDTOFIX", gui.init_data.render_pass, swapChainExtent, &clearValues, 0, singleTriangularVertexBuffer, 0, 3, 0, 0, 0);
	}
	*/

	void createMainRenderPass();
	void createMainFramebuffers();
	void createMainCommandPool();
	void createMainCommandBuffers();
	void recordMainCommandBuffers();

	void reCreateRenderer();

	void assignRenderpassID(Material* pMat, VkRenderPass renderPass, uint32_t postProcessIndex = 0);

	VkRenderPass getAssignedRenderpassID(Material* pMat);

	void releaseRenderPart();

	void mainloop();

	void updateUniformBuffers(unsigned int deltaTime)
	{
		//mainCamera.updateCameraBuffer();
	}

	void draw(unsigned int deltaTime);
	void shutDown();

	void createPointLightBuffer();
	void updatePointLightBuffer();

	void createDirectionalLightBuffer();
	void updateDirectionalLightBuffer();

	void createSSRBuffer();
	void updateSSRBuffer();

	void createSSRInfoBuffer();
	void updateSSRInfoBuffer();

	void createPerFrameBuffer();
	void updatePerFrameBuffer();

	void culling();

	Camera mainCamera;
	

	void setGlobalObjs();
	void setGlobalLights();

private:

	Vulkan *vulkanApp;

	Interface interface;

	/*
	GUI gui;
	std::vector<Texture*> guiCanvas;
	std::vector<VkFramebuffer> guiFramebuffers;
	VkCommandPool guiCmdPool;
	std::vector<VkCommandBuffer> guiCmd;
	*/

	Sky skySystem;

	int layerCount;

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	Texture *depthTexture;
	
	/*
	Texture *depthMipmapTexture;

	std::vector<VkBuffer> depthMipSizeBuffer;
	std::vector<VkDeviceMemory> depthMipSizeBufferMemory;
	*/

	Texture *SSRDepthTexture;	

	std::vector<Texture*> gbuffers;
	std::vector<VkFramebuffer> gbufferFramebuffers;

	VkSwapchainKHR swapChain;

	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	
	VkRenderPass gbufferRenderPass;
	VkCommandPool gbufferCmdPool;
	std::vector<VkCommandBuffer> gbufferCmd;
	
	VkCommandPool frustumCullingPool;
	std::vector<VkCommandBuffer> frustumCmd;

	VkRenderPass mainRenderPass;
	VkCommandPool mainCmdPool;
	std::vector<VkCommandBuffer> mainCmd;

	VkBuffer singleTriangularVertexBuffer;
	VkDeviceMemory singleTriangularVertexMemory;

	

	VkSemaphore gbufferSemaphore;
	VkSemaphore pbrSemaphore;	
	VkSemaphore guiSemaphore;
	VkSemaphore presentSemaphore;
	
	VkQueue frustumQueue;
	VkQueue gbufferQueue;
	VkQueue pbrQueue;
	VkQueue guiQueue;
	VkQueue presentQueue;

	FrustumCullingMaterial* pfrustumCullingMaterial;

	std::vector<DirectionalLight*> directionalLights;
	std::vector<LightInfo> directionalLightInfo;

	VkBuffer directionalLightUniformBuffer;
	VkDeviceMemory directionalLightUniformMemory;

	std::vector<PointLight*> pointLights;
	std::vector<LightInfo> pointLightInfo;

	VkBuffer pointLightUniformBuffer;
	VkDeviceMemory pointLightUniformMemory;

	//SSRDepthInfo *SSROforReset;
	//VkBuffer SSROffsetBuffer;
	//VkDeviceMemory SSROffsetBufferMemory;

	//uint32_t *SSRDforReset;
	VkBuffer SSRDepthBuffer;	
	VkDeviceMemory SSRDepthBufferMemory;

	VkBuffer perFrameBuffer;
	VkDeviceMemory perFrameBufferMemory;

	//glm::vec4 *screenSpaceNoise;

	VkBuffer SSRInfoBuffer;
	VkDeviceMemory SSRInfoBufferMem;

	std::vector<PostProcess*> postProcessChain;
};