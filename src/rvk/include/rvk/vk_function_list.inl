#ifdef VK_EXPORT_FUNCTION
VK_EXPORT_FUNCTION(GetInstanceProcAddr)
#endif /*VK_EXPORT_FUNCTION*/

/*
** GLOBAL
*/
#ifdef VK_GLOBAL_LEVEL_FUNCTION
VK_GLOBAL_LEVEL_FUNCTION(CreateInstance)
VK_GLOBAL_LEVEL_FUNCTION(EnumerateInstanceExtensionProperties)
VK_GLOBAL_LEVEL_FUNCTION(EnumerateInstanceLayerProperties)
#endif /*VK_GLOBAL_LEVEL_FUNCTION*/

/*
** INSTANCE
*/
#ifdef VK_INSTANCE_LEVEL_FUNCTION
VK_INSTANCE_LEVEL_FUNCTION(GetDeviceProcAddr)
VK_INSTANCE_LEVEL_FUNCTION(CreateDevice)
VK_INSTANCE_LEVEL_FUNCTION(DestroyInstance)
VK_INSTANCE_LEVEL_FUNCTION(EnumeratePhysicalDevices)
VK_INSTANCE_LEVEL_FUNCTION(EnumeratePhysicalDeviceGroups)
VK_INSTANCE_LEVEL_FUNCTION(EnumerateDeviceExtensionProperties)
VK_INSTANCE_LEVEL_FUNCTION(EnumerateDeviceLayerProperties)
VK_INSTANCE_LEVEL_FUNCTION(EnumerateInstanceVersion)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceFeatures)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceFormatProperties)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceImageFormatProperties)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceProperties)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceQueueFamilyProperties)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceMemoryProperties)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceSparseImageFormatProperties)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceFeatures2)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceProperties2)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceFormatProperties2)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceImageFormatProperties2)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceQueueFamilyProperties2)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceMemoryProperties2)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceSparseImageFormatProperties2)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceExternalBufferProperties)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceExternalFenceProperties)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceExternalSemaphoreProperties)
#ifdef VK_KHR_surface
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceSurfaceCapabilitiesKHR)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceSurfaceFormatsKHR)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceSurfacePresentModesKHR)
VK_INSTANCE_LEVEL_FUNCTION(GetPhysicalDeviceSurfaceSupportKHR)
#if defined( _WIN32 )
VK_INSTANCE_LEVEL_FUNCTION(CreateWin32SurfaceKHR)
#elif defined(__APPLE__)
VK_INSTANCE_LEVEL_FUNCTION(CreateMacOSSurfaceMVK)
#elif defined( __linux__ )
#if defined( VK_USE_PLATFORM_WAYLAND_KHR )
VK_INSTANCE_LEVEL_FUNCTION(CreateWaylandSurfaceKHR)
#elif defined( VK_USE_PLATFORM_XLIB_KHR )
VK_INSTANCE_LEVEL_FUNCTION(CreateXlibSurfaceKHR)
#elif defined( VK_USE_PLATFORM_XCB_KHR )
VK_INSTANCE_LEVEL_FUNCTION(CreateXcbSurfaceKHR)
#endif /*linux*/
#endif /*platform*/
VK_INSTANCE_LEVEL_FUNCTION(DestroySurfaceKHR)
#endif /*VK_INSTANCE_LEVEL_FUNCTION*/
    
/* Debug */
#ifndef NDEBUG
VK_INSTANCE_LEVEL_FUNCTION(CreateDebugUtilsMessengerEXT)
VK_INSTANCE_LEVEL_FUNCTION(DestroyDebugUtilsMessengerEXT)
#endif /*NDEBUG*/
#endif /*VK_INSTANCE_LEVEL_FUNCTION*/

/*
** DEVICE
*/
#ifdef VK_DEVICE_LEVEL_FUNCTION
VK_DEVICE_LEVEL_FUNCTION(DestroyDevice)
VK_DEVICE_LEVEL_FUNCTION(GetDeviceQueue)
VK_DEVICE_LEVEL_FUNCTION(QueueSubmit)
VK_DEVICE_LEVEL_FUNCTION(QueueWaitIdle)
VK_DEVICE_LEVEL_FUNCTION(DeviceWaitIdle)
VK_DEVICE_LEVEL_FUNCTION(AllocateMemory)
VK_DEVICE_LEVEL_FUNCTION(FreeMemory)
VK_DEVICE_LEVEL_FUNCTION(MapMemory)
VK_DEVICE_LEVEL_FUNCTION(UnmapMemory)
VK_DEVICE_LEVEL_FUNCTION(FlushMappedMemoryRanges)
VK_DEVICE_LEVEL_FUNCTION(InvalidateMappedMemoryRanges)
VK_DEVICE_LEVEL_FUNCTION(GetDeviceMemoryCommitment)
VK_DEVICE_LEVEL_FUNCTION(BindBufferMemory)
VK_DEVICE_LEVEL_FUNCTION(BindImageMemory)
VK_DEVICE_LEVEL_FUNCTION(GetBufferMemoryRequirements)
VK_DEVICE_LEVEL_FUNCTION(GetImageMemoryRequirements)
VK_DEVICE_LEVEL_FUNCTION(GetImageSparseMemoryRequirements)
VK_DEVICE_LEVEL_FUNCTION(QueueBindSparse)
VK_DEVICE_LEVEL_FUNCTION(CreateFence)
VK_DEVICE_LEVEL_FUNCTION(DestroyFence)
VK_DEVICE_LEVEL_FUNCTION(ResetFences)
VK_DEVICE_LEVEL_FUNCTION(GetFenceStatus)
VK_DEVICE_LEVEL_FUNCTION(WaitForFences)
VK_DEVICE_LEVEL_FUNCTION(CreateSemaphore)
VK_DEVICE_LEVEL_FUNCTION(DestroySemaphore)
VK_DEVICE_LEVEL_FUNCTION(CreateEvent)
VK_DEVICE_LEVEL_FUNCTION(DestroyEvent)
VK_DEVICE_LEVEL_FUNCTION(GetEventStatus)
VK_DEVICE_LEVEL_FUNCTION(SetEvent)
VK_DEVICE_LEVEL_FUNCTION(ResetEvent)
VK_DEVICE_LEVEL_FUNCTION(CreateQueryPool)
VK_DEVICE_LEVEL_FUNCTION(DestroyQueryPool)
VK_DEVICE_LEVEL_FUNCTION(GetQueryPoolResults)
VK_DEVICE_LEVEL_FUNCTION(CreateBuffer)
VK_DEVICE_LEVEL_FUNCTION(DestroyBuffer)
VK_DEVICE_LEVEL_FUNCTION(CreateBufferView)
VK_DEVICE_LEVEL_FUNCTION(DestroyBufferView)
VK_DEVICE_LEVEL_FUNCTION(CreateImage)
VK_DEVICE_LEVEL_FUNCTION(DestroyImage)
VK_DEVICE_LEVEL_FUNCTION(GetImageSubresourceLayout)
VK_DEVICE_LEVEL_FUNCTION(CreateImageView)
VK_DEVICE_LEVEL_FUNCTION(DestroyImageView)
VK_DEVICE_LEVEL_FUNCTION(CreateShaderModule)
VK_DEVICE_LEVEL_FUNCTION(DestroyShaderModule)
VK_DEVICE_LEVEL_FUNCTION(CreatePipelineCache)
VK_DEVICE_LEVEL_FUNCTION(DestroyPipelineCache)
VK_DEVICE_LEVEL_FUNCTION(GetPipelineCacheData)
VK_DEVICE_LEVEL_FUNCTION(MergePipelineCaches)
VK_DEVICE_LEVEL_FUNCTION(CreateGraphicsPipelines)
VK_DEVICE_LEVEL_FUNCTION(CreateComputePipelines)
VK_DEVICE_LEVEL_FUNCTION(DestroyPipeline)
VK_DEVICE_LEVEL_FUNCTION(CreatePipelineLayout)
VK_DEVICE_LEVEL_FUNCTION(DestroyPipelineLayout)
VK_DEVICE_LEVEL_FUNCTION(CreateSampler)
VK_DEVICE_LEVEL_FUNCTION(DestroySampler)
VK_DEVICE_LEVEL_FUNCTION(CreateDescriptorSetLayout)
VK_DEVICE_LEVEL_FUNCTION(DestroyDescriptorSetLayout)
VK_DEVICE_LEVEL_FUNCTION(CreateDescriptorPool)
VK_DEVICE_LEVEL_FUNCTION(DestroyDescriptorPool)
VK_DEVICE_LEVEL_FUNCTION(ResetDescriptorPool)
VK_DEVICE_LEVEL_FUNCTION(AllocateDescriptorSets)
VK_DEVICE_LEVEL_FUNCTION(FreeDescriptorSets)
VK_DEVICE_LEVEL_FUNCTION(UpdateDescriptorSets)
VK_DEVICE_LEVEL_FUNCTION(CreateFramebuffer)
VK_DEVICE_LEVEL_FUNCTION(DestroyFramebuffer)
VK_DEVICE_LEVEL_FUNCTION(CreateRenderPass)
VK_DEVICE_LEVEL_FUNCTION(DestroyRenderPass)
VK_DEVICE_LEVEL_FUNCTION(GetRenderAreaGranularity)
VK_DEVICE_LEVEL_FUNCTION(CreateCommandPool)
VK_DEVICE_LEVEL_FUNCTION(DestroyCommandPool)
VK_DEVICE_LEVEL_FUNCTION(ResetCommandPool)
VK_DEVICE_LEVEL_FUNCTION(AllocateCommandBuffers)
VK_DEVICE_LEVEL_FUNCTION(FreeCommandBuffers)
VK_DEVICE_LEVEL_FUNCTION(BeginCommandBuffer)
VK_DEVICE_LEVEL_FUNCTION(EndCommandBuffer)
VK_DEVICE_LEVEL_FUNCTION(ResetCommandBuffer)
VK_DEVICE_LEVEL_FUNCTION(BindBufferMemory2)
VK_DEVICE_LEVEL_FUNCTION(BindImageMemory2)
VK_DEVICE_LEVEL_FUNCTION(GetDeviceGroupPeerMemoryFeatures)
VK_DEVICE_LEVEL_FUNCTION(GetImageMemoryRequirements2)
VK_DEVICE_LEVEL_FUNCTION(GetBufferMemoryRequirements2)
VK_DEVICE_LEVEL_FUNCTION(GetImageSparseMemoryRequirements2)
VK_DEVICE_LEVEL_FUNCTION(TrimCommandPool)
VK_DEVICE_LEVEL_FUNCTION(GetDeviceQueue2)
VK_DEVICE_LEVEL_FUNCTION(CreateSamplerYcbcrConversion)
VK_DEVICE_LEVEL_FUNCTION(DestroySamplerYcbcrConversion)
VK_DEVICE_LEVEL_FUNCTION(CreateDescriptorUpdateTemplate)
VK_DEVICE_LEVEL_FUNCTION(DestroyDescriptorUpdateTemplate)
VK_DEVICE_LEVEL_FUNCTION(UpdateDescriptorSetWithTemplate)
VK_DEVICE_LEVEL_FUNCTION(GetDescriptorSetLayoutSupport)
VK_DEVICE_LEVEL_FUNCTION(CreateRenderPass2)
VK_DEVICE_LEVEL_FUNCTION(ResetQueryPool)
VK_DEVICE_LEVEL_FUNCTION(GetSemaphoreCounterValue)
VK_DEVICE_LEVEL_FUNCTION(WaitSemaphores)
VK_DEVICE_LEVEL_FUNCTION(SignalSemaphore)
VK_DEVICE_LEVEL_FUNCTION(GetBufferDeviceAddress)
VK_DEVICE_LEVEL_FUNCTION(GetBufferOpaqueCaptureAddress)
VK_DEVICE_LEVEL_FUNCTION(GetDeviceMemoryOpaqueCaptureAddress)
#endif /*VK_DEVICE_LEVEL_FUNCTION*/
#ifdef VK_DEVICE_LEVEL_CMD_FUNCTION
VK_DEVICE_LEVEL_CMD_FUNCTION(BindPipeline)
VK_DEVICE_LEVEL_CMD_FUNCTION(SetViewport)
VK_DEVICE_LEVEL_CMD_FUNCTION(SetScissor)
VK_DEVICE_LEVEL_CMD_FUNCTION(SetLineWidth)
VK_DEVICE_LEVEL_CMD_FUNCTION(SetDepthBias)
VK_DEVICE_LEVEL_CMD_FUNCTION(SetBlendConstants)
VK_DEVICE_LEVEL_CMD_FUNCTION(SetDepthBounds)
VK_DEVICE_LEVEL_CMD_FUNCTION(SetStencilCompareMask)
VK_DEVICE_LEVEL_CMD_FUNCTION(SetStencilWriteMask)
VK_DEVICE_LEVEL_CMD_FUNCTION(SetStencilReference)
VK_DEVICE_LEVEL_CMD_FUNCTION(BindDescriptorSets)
VK_DEVICE_LEVEL_CMD_FUNCTION(BindIndexBuffer)
VK_DEVICE_LEVEL_CMD_FUNCTION(BindVertexBuffers)
VK_DEVICE_LEVEL_CMD_FUNCTION(Draw)
VK_DEVICE_LEVEL_CMD_FUNCTION(DrawIndexed)
VK_DEVICE_LEVEL_CMD_FUNCTION(DrawIndirect)
VK_DEVICE_LEVEL_CMD_FUNCTION(DrawIndexedIndirect)
VK_DEVICE_LEVEL_CMD_FUNCTION(Dispatch)
VK_DEVICE_LEVEL_CMD_FUNCTION(DispatchIndirect)
VK_DEVICE_LEVEL_CMD_FUNCTION(CopyBuffer)
VK_DEVICE_LEVEL_CMD_FUNCTION(CopyImage)
VK_DEVICE_LEVEL_CMD_FUNCTION(BlitImage)
VK_DEVICE_LEVEL_CMD_FUNCTION(CopyBufferToImage)
VK_DEVICE_LEVEL_CMD_FUNCTION(CopyImageToBuffer)
VK_DEVICE_LEVEL_CMD_FUNCTION(UpdateBuffer)
VK_DEVICE_LEVEL_CMD_FUNCTION(FillBuffer)
VK_DEVICE_LEVEL_CMD_FUNCTION(ClearColorImage)
VK_DEVICE_LEVEL_CMD_FUNCTION(ClearDepthStencilImage)
VK_DEVICE_LEVEL_CMD_FUNCTION(ClearAttachments)
VK_DEVICE_LEVEL_CMD_FUNCTION(ResolveImage)
VK_DEVICE_LEVEL_CMD_FUNCTION(SetEvent)
VK_DEVICE_LEVEL_CMD_FUNCTION(ResetEvent)
VK_DEVICE_LEVEL_CMD_FUNCTION(WaitEvents)
VK_DEVICE_LEVEL_CMD_FUNCTION(PipelineBarrier)
VK_DEVICE_LEVEL_CMD_FUNCTION(BeginQuery)
VK_DEVICE_LEVEL_CMD_FUNCTION(EndQuery)
VK_DEVICE_LEVEL_CMD_FUNCTION(ResetQueryPool)
VK_DEVICE_LEVEL_CMD_FUNCTION(WriteTimestamp)
VK_DEVICE_LEVEL_CMD_FUNCTION(CopyQueryPoolResults)
VK_DEVICE_LEVEL_CMD_FUNCTION(PushConstants)
VK_DEVICE_LEVEL_CMD_FUNCTION(BeginRenderPass)
VK_DEVICE_LEVEL_CMD_FUNCTION(NextSubpass)
VK_DEVICE_LEVEL_CMD_FUNCTION(EndRenderPass)
VK_DEVICE_LEVEL_CMD_FUNCTION(ExecuteCommands)
VK_DEVICE_LEVEL_CMD_FUNCTION(SetDeviceMask)
VK_DEVICE_LEVEL_CMD_FUNCTION(DispatchBase)
VK_DEVICE_LEVEL_CMD_FUNCTION(DrawIndirectCount)
VK_DEVICE_LEVEL_CMD_FUNCTION(DrawIndexedIndirectCount)
VK_DEVICE_LEVEL_CMD_FUNCTION(BeginRenderPass2)
VK_DEVICE_LEVEL_CMD_FUNCTION(NextSubpass2)
VK_DEVICE_LEVEL_CMD_FUNCTION(EndRenderPass2)
#endif /*VK_DEVICE_LEVEL_CMD_FUNCTION*/
/*
** EXT Debug Utils
*/
#ifdef VK_EXT_debug_utils
#ifdef VK_DEVICE_LEVEL_CMD_FUNCTION
VK_DEVICE_LEVEL_CMD_FUNCTION(BeginDebugUtilsLabelEXT)
VK_DEVICE_LEVEL_CMD_FUNCTION(EndDebugUtilsLabelEXT)
#endif /*VK_DEVICE_LEVEL_CMD_FUNCTION*/
#endif /*VK_EXT_debug_utils*/
/*
** KHR Swapchain
*/
#ifdef VK_KHR_swapchain
#ifdef VK_DEVICE_LEVEL_FUNCTION
VK_DEVICE_LEVEL_FUNCTION(AcquireNextImageKHR)
VK_DEVICE_LEVEL_FUNCTION(CreateSwapchainKHR)
VK_DEVICE_LEVEL_FUNCTION(DestroySwapchainKHR)
VK_DEVICE_LEVEL_FUNCTION(GetSwapchainImagesKHR)
VK_DEVICE_LEVEL_FUNCTION(QueuePresentKHR)
#endif /*VK_DEVICE_LEVEL_FUNCTION*/
#endif /*VK_KHR_swapchain*/
/*
** KHR Ray Tracing
*/
#ifdef VK_KHR_ray_tracing_pipeline
#ifdef VK_DEVICE_LEVEL_FUNCTION
VK_DEVICE_LEVEL_FUNCTION(CreateRayTracingPipelinesKHR)
VK_DEVICE_LEVEL_FUNCTION(GetRayTracingShaderGroupHandlesKHR)
VK_DEVICE_LEVEL_FUNCTION(GetRayTracingShaderGroupStackSizeKHR)
#endif /*VK_DEVICE_LEVEL_FUNCTION*/
#ifdef VK_DEVICE_LEVEL_CMD_FUNCTION
VK_DEVICE_LEVEL_CMD_FUNCTION(SetRayTracingPipelineStackSizeKHR)
VK_DEVICE_LEVEL_CMD_FUNCTION(TraceRaysKHR)
VK_DEVICE_LEVEL_CMD_FUNCTION(TraceRaysIndirectKHR)
#endif /*VK_DEVICE_LEVEL_CMD_FUNCTION*/
#endif /*VK_KHR_ray_tracing_pipeline*/

#ifdef VK_KHR_acceleration_structure
#ifdef VK_DEVICE_LEVEL_FUNCTION
VK_DEVICE_LEVEL_FUNCTION(BuildAccelerationStructuresKHR)
VK_DEVICE_LEVEL_FUNCTION(CreateAccelerationStructureKHR)
VK_DEVICE_LEVEL_FUNCTION(DestroyAccelerationStructureKHR)
VK_DEVICE_LEVEL_FUNCTION(GetAccelerationStructureBuildSizesKHR)
VK_DEVICE_LEVEL_FUNCTION(GetAccelerationStructureDeviceAddressKHR)
#endif /*VK_DEVICE_LEVEL_FUNCTION*/
#ifdef VK_DEVICE_LEVEL_CMD_FUNCTION
VK_DEVICE_LEVEL_CMD_FUNCTION(BuildAccelerationStructuresKHR)
VK_DEVICE_LEVEL_CMD_FUNCTION(CopyAccelerationStructureKHR)
VK_DEVICE_LEVEL_CMD_FUNCTION(WriteAccelerationStructuresPropertiesKHR)
#endif /*VK_DEVICE_LEVEL_CMD_FUNCTION*/
#endif /*VK_KHR_acceleration_structure*/

/*
** KHR External
*/
#ifdef VK_DEVICE_LEVEL_FUNCTION
#if defined( _WIN32 )
#ifdef VK_KHR_external_memory_win32
VK_DEVICE_LEVEL_FUNCTION(GetMemoryWin32HandleKHR)
VK_DEVICE_LEVEL_FUNCTION(GetMemoryWin32HandlePropertiesKHR)
#endif /*VK_KHR_external_memory_win32*/
#ifdef VK_KHR_external_semaphore_win32
VK_DEVICE_LEVEL_FUNCTION(ImportSemaphoreWin32HandleKHR)
VK_DEVICE_LEVEL_FUNCTION(GetSemaphoreWin32HandleKHR)
#endif /*VK_KHR_external_semaphore_win32*/
#ifdef VK_KHR_external_fence_win32
VK_DEVICE_LEVEL_FUNCTION(ImportFenceWin32HandleKHR)
VK_DEVICE_LEVEL_FUNCTION(GetFenceWin32HandleKHR)
#endif /*VK_KHR_external_fence_win32*/
#elif defined( __linux__ ) || defined(__APPLE__)
#ifdef VK_KHR_external_memory_fd
VK_DEVICE_LEVEL_FUNCTION(GetMemoryFdKHR)
VK_DEVICE_LEVEL_FUNCTION(GetMemoryFdPropertiesKHR)
#endif /*VK_KHR_external_memory_fd*/
#ifdef VK_KHR_external_semaphore_fd
VK_DEVICE_LEVEL_FUNCTION(ImportSemaphoreFdKHR)
VK_DEVICE_LEVEL_FUNCTION(GetSemaphoreFdKHR)
#endif /*VK_KHR_external_semaphore_fd*/
#ifdef VK_KHR_external_fence_fd
VK_DEVICE_LEVEL_FUNCTION(ImportFenceFdKHR)
VK_DEVICE_LEVEL_FUNCTION(GetFenceFdKHR)
#endif /*VK_KHR_external_fence_fd*/
#endif /*platform*/
#endif /*VK_DEVICE_LEVEL_FUNCTION*/

/*
** NV Cuda
*/
#ifdef VK_NVX_binary_import
#ifdef VK_DEVICE_LEVEL_FUNCTION
VK_DEVICE_LEVEL_FUNCTION(CreateCuModuleNVX)
VK_DEVICE_LEVEL_FUNCTION(CreateCuFunctionNVX)
VK_DEVICE_LEVEL_FUNCTION(DestroyCuModuleNVX)
VK_DEVICE_LEVEL_FUNCTION(DestroyCuFunctionNVX)
#endif /*VK_DEVICE_LEVEL_FUNCTION*/
#ifdef VK_DEVICE_LEVEL_CMD_FUNCTION
VK_DEVICE_LEVEL_CMD_FUNCTION(CuLaunchKernelNVX)
#endif /*VK_DEVICE_LEVEL_CMD_FUNCTION*/
#endif /*VK_NVX_binary_import*/
