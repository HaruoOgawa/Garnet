#include "CMaterialFrame.h"

namespace graphics
{
	CMaterialFrame::CMaterialFrame():
		m_CreateCounter(0),
		m_MaterialName(std::string()),
		m_CreateInfo(nullptr)
	{
	}

	CMaterialFrame::~CMaterialFrame()
	{
	}

	void CMaterialFrame::SetMaterialName(const std::string& Name)
	{
		m_MaterialName = Name;
	}

	void CMaterialFrame::SetCreateInfo(const std::shared_ptr<graphics::CMaterialCreateInfo>& CreateInfo)
	{
		m_CreateInfo = CreateInfo;
	}

	void CMaterialFrame::SetShaderBufferList(const std::vector<SShaderBufferSet>& ShaderBufferList)
	{
		m_ShaderBufferList = ShaderBufferList;
	}

	void CMaterialFrame::SetTextureBufferList(const std::vector<graphics::STextureBindingLayout>& TextureBufferList)
	{
		m_TextureBufferList = TextureBufferList;
	}

	std::shared_ptr<CMaterial> CMaterialFrame::CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, int RefCount, graphics::ECullMode CullMode)
	{
		if (!m_CreateInfo) return nullptr;

		std::shared_ptr<CMaterial> Material = pGraphicsAPI->CreateMaterial(m_CreateInfo, RefCount, CullMode);

		// MaterialName
		std::string MaterialName = m_MaterialName + "_" + std::to_string(m_CreateCounter);
		Material->SetMaterialName(MaterialName);

		// ShaderBuffer
		for (const auto& ShaderBuffer : m_ShaderBufferList)
		{
			if (ShaderBuffer.BufferType == graphics::EBufferType::UNIFORM)
			{
				// UniformBuffer
				auto BindingLayout = ShaderBuffer.BindingLayout;
				const auto& ValueList = ShaderBuffer.ValueLayoutList;

				auto Buffer = graphics::CMaterialCreateInfo::CreateUniformBuffer(ShaderBuffer.BindingLayout.BindingName, { BindingLayout });

				for (const auto& Value : ValueList)
				{
					Buffer->AddData(Value->Name, Value->ValueType, &Value->Data[0], Value->ByteSize, Value->BindingIndex, Value->ValueInput);
				}

				Material->AddShaderBuffer(Buffer);
			}
			else if (ShaderBuffer.BufferType == graphics::EBufferType::SHADERSTORAGE)
			{
				// StorageBuffer
				auto BindingLayout = ShaderBuffer.BindingLayout;
				const auto& ValueList = ShaderBuffer.ValueLayoutList;

				auto Buffer = graphics::CMaterialCreateInfo::CreateShaderStorageBuffer(ShaderBuffer.BindingLayout.BindingName, { BindingLayout }, BindingLayout.BufferUpdateType);

				for (const auto& Value : ValueList)
				{
					Buffer->AddData(Value->Name, Value->ValueType, &Value->Data[0], Value->ByteSize, Value->BindingIndex, Value->ValueInput);
				}

				Material->AddShaderBuffer(Buffer);
			}
		}

		// TextureBuffer
		for (const auto& TextureBuffer : m_TextureBufferList)
		{
			Material->AddTextureBindingLayout({ TextureBuffer.TextureName, TextureBuffer.ViewBindingIndex, TextureBuffer.SamplerBindingIndex,TextureBuffer.TextureIndex,TextureBuffer.TextureUsage });
		}

		// カウンターを更新
		m_CreateCounter++;

		m_RefMaterialList.push_back(Material);

		return Material;
	}

	bool CMaterialFrame::Reload()
	{
		std::vector<std::shared_ptr<CShaderBuffer>> ShaderBufferList;
		std::vector<STextureBindingLayout> TextureBindingLayoutList;

		// ShaderBuffer
		for (const auto& ShaderBuffer : m_ShaderBufferList)
		{
			if (ShaderBuffer.BufferType == graphics::EBufferType::UNIFORM)
			{
				// UniformBuffer
				auto BindingLayout = ShaderBuffer.BindingLayout;
				const auto& ValueList = ShaderBuffer.ValueLayoutList;

				auto Buffer = graphics::CMaterialCreateInfo::CreateUniformBuffer(ShaderBuffer.BindingLayout.BindingName, { BindingLayout });

				for (const auto& Value : ValueList)
				{
					Buffer->AddData(Value->Name, Value->ValueType, &Value->Data[0], Value->ByteSize, Value->BindingIndex, Value->ValueInput);
				}

				Buffer->RecalculateBindingLayoutOffset();
				Buffer->ResizePowerOfTwo(); // バッファサイズを2のn乗にする

				ShaderBufferList.push_back(Buffer);
			}
			else if (ShaderBuffer.BufferType == graphics::EBufferType::SHADERSTORAGE)
			{
				// StorageBuffer
				auto BindingLayout = ShaderBuffer.BindingLayout;
				const auto& ValueList = ShaderBuffer.ValueLayoutList;

				auto Buffer = graphics::CMaterialCreateInfo::CreateShaderStorageBuffer(ShaderBuffer.BindingLayout.BindingName, { BindingLayout }, BindingLayout.BufferUpdateType);

				for (const auto& Value : ValueList)
				{
					Buffer->AddData(Value->Name, Value->ValueType, &Value->Data[0], Value->ByteSize, Value->BindingIndex, Value->ValueInput);
				}

				Buffer->RecalculateBindingLayoutOffset();
				Buffer->ResizePowerOfTwo(); // バッファサイズを2のn乗にする

				ShaderBufferList.push_back(Buffer);
			}
		}

		// TextureBuffer
		for (const auto& TextureBuffer : m_TextureBufferList)
		{
			TextureBindingLayoutList.push_back({ TextureBuffer.TextureName, TextureBuffer.ViewBindingIndex, TextureBuffer.SamplerBindingIndex,TextureBuffer.TextureIndex,TextureBuffer.TextureUsage });
		}

		//
		for (auto& Material : m_RefMaterialList)
		{
			// APIレベルでマテリアルを更新する
			if (!Material->ReCreate(m_CreateInfo, ShaderBufferList, TextureBindingLayoutList)) return false;
		}

		return true;
	}
}