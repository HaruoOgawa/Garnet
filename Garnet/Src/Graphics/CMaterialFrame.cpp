#include "CMaterialFrame.h"

namespace graphics
{
	CMaterialFrame::CMaterialFrame()
	{
	}

	CMaterialFrame::~CMaterialFrame()
	{
	}

	void CMaterialFrame::SetCreateInfo(const std::shared_ptr<graphics::CMaterialCreateInfo>& CreateInfo)
	{
		m_CreateInfo = CreateInfo;
	}

	void CMaterialFrame::SetUniformBufferList(const std::vector<std::pair<graphics::SBindingLayout, std::vector<std::shared_ptr<graphics::SBufferValueLayout>>>>& UniformBufferList)
	{
		m_UniformBufferList = UniformBufferList;
	}

	void CMaterialFrame::SetStorageBufferList(const std::vector<std::pair<graphics::SBindingLayout, std::vector<std::shared_ptr<graphics::SBufferValueLayout>>>>& StorageBufferList)
	{
		m_StorageBufferList = StorageBufferList;
	}

	void CMaterialFrame::SetTextureBufferList(const std::vector<graphics::STextureBindingLayout>& TextureBufferList)
	{
		m_TextureBufferList = TextureBufferList;
	}

	std::shared_ptr<CMaterial> CMaterialFrame::CreateMaterial(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_CreateInfo) return nullptr;

		std::shared_ptr<CMaterial> Material = pGraphicsAPI->CreateMaterial(m_CreateInfo);

		// UniformBuffer
		for (const auto& UniformBuffer : m_UniformBufferList)
		{
			auto BindingLayout = UniformBuffer.first;
			const auto& ValueList = UniformBuffer.second;

			auto Buffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ BindingLayout });

			for (const auto& Value : ValueList)
			{
				Buffer->AddData(Value->Name, &Value->Data[0], Value->ByteSize, Value->BindingIndex);
			}

			Material->AddShaderBuffer(Buffer);
		}

		// StorageBuffer
		for (const auto& StorageBuffer : m_StorageBufferList)
		{
			const auto& BindingLayout = StorageBuffer.first;
			const auto& ValueList = StorageBuffer.second;

			auto Buffer = graphics::CMaterialCreateInfo::CreateShaderStorageBuffer({ BindingLayout }, BindingLayout.BufferUpdateType);

			for (const auto& Value : ValueList)
			{
				Buffer->AddData(Value->Name, &Value->Data[0], Value->ByteSize, Value->BindingIndex);
			}

			Material->AddShaderBuffer(Buffer);
		}

		// TextureBuffer
		for (const auto& TextureBuffer : m_TextureBufferList)
		{
			Material->AddTextureBindingLayout({ TextureBuffer.TextureName, TextureBuffer.ViewBindingIndex, TextureBuffer.SamplerBindingIndex,TextureBuffer.TextureIndex,TextureBuffer.TextureUsage });
		}

		return Material;
	}
}