#pragma once
#include <rvk/parts/rvk_public.hpp>

#include <optional>
#include <vector>

RVK_BEGIN_NAMESPACE
class LogicalDevice;
class Renderpass;
class AttachmentDescription {
public:
													AttachmentDescription(VkFormat aFormat, VkImageLayout aInit, VkImageLayout aFinal);
	void											setAttachmentOps(VkAttachmentLoadOp aLoad, VkAttachmentStoreOp aStore);
	void											setAttachmentStencilOps(VkAttachmentLoadOp aLoad, VkAttachmentStoreOp aStore);

private:
	friend class									Renderpass;
	VkAttachmentDescription							attachment;
};

class Renderpass {
public:
													Renderpass(LogicalDevice* aDevice);

													~Renderpass();

	void											reserve(int aAttachmentsReserve, int aSubpassesReserve, int aDependenciesReserve);
													// add subpasses/attachments for later linking
	void											addAttachment(const AttachmentDescription& aAttachmentDescription);
	void											addSubpass(VkPipelineBindPoint aPipelineBindPoint, int aColorAttachmentReserveCount = 1);
													// link subpasses/attachments and specify the layout during execution
													// also decide if an attachment is a color/depth or stencil attachment
	void											setColorAttachment(int aSubpassIdx, int aAttachmentIdx, VkImageLayout aDuring);
	void											setDepthStencilAttachment(int aSubpassIdx, int aAttachmentIdx, VkImageLayout aDuring);
													// add dependencies between subpasses
	void											addDependency(uint32_t aSrcSubpass, uint32_t aDstSubpass,
																	VkPipelineStageFlags aSrcStageMask, VkPipelineStageFlags aDstStageMask,
																	VkAccessFlags aSrcAccessMask, VkAccessFlags aDstAccessMask, VkDependencyFlags aDependencyFlag);
	void											finish();
	void											destroy();

	VkRenderPass									getHandle() const;
private:
	void											buildSubpassDescriptions();

	friend class									Framebuffer;
	friend class									Swapchain;

	LogicalDevice*									mDevice;
	// attachment
	std::vector<VkAttachmentDescription>			mAttachments;
	// subpass
	struct Subpass_s {
		VkPipelineBindPoint							pipelineBindPoint;
		std::vector<VkAttachmentReference>			colorAttachmentRefs;
		std::optional<VkAttachmentReference>		depthAttachmentRef;
	};
	std::vector<Subpass_s>							mSubpasses;
	std::vector<VkSubpassDescription>				mSubpassDescriptions;
	// dependency
	std::vector<VkSubpassDependency>				mDependencies;
	// renderpass
	VkRenderPass									mRenderPass;
};
RVK_END_NAMESPACE