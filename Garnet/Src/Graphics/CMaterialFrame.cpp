#include "CMaterialFrame.h"

namespace graphics
{
	CMaterialFrame::CMaterialFrame():
		m_CreateCounter(0),
		m_MaterialFrameName(std::string()),
		m_FileName(std::string()),
		m_CreateInfo(nullptr),
		m_OutputColorCount(1)
	{
	}

	CMaterialFrame::~CMaterialFrame()
	{
	}

	void CMaterialFrame::SetMaterialFrameName(const std::string& Name)
	{
		m_MaterialFrameName = Name;
	}

	const std::string& CMaterialFrame::GetMaterialFrameName() const
	{
		return m_MaterialFrameName;
	}

	void CMaterialFrame::SetFileName(const std::string& Name)
	{
		m_FileName = Name;
	}

	const std::string& CMaterialFrame::GetFileName() const
	{
		return m_FileName;
	}

	bool CMaterialFrame::GetDefaultValue(const std::string& BufferName, const std::string& UniformName, std::vector<float>& DstData) const
	{
		if (m_ShaderBufferList.empty()) return false;

		for (const auto& ShaderBuffer : m_ShaderBufferList)
		{
			if (ShaderBuffer.BindingLayout.BindingName != BufferName) continue;

			const auto& ValueList = ShaderBuffer.ValueLayoutList;

			for (const auto& Value : ValueList)
			{
				if (Value->Name != UniformName) continue;

				// 対象のUniformを見つけた
				std::vector<float> Data = Value->Data;
				DstData = Data;

				return true;
			}
		}

		return false;
	}

	// カラーバッファへのアウトプット数(MRTで使用)
	void CMaterialFrame::SetOutputColorCount(int Val)
	{
		m_OutputColorCount = Val;
	}

	int CMaterialFrame::GetOutputColorCount() const
	{
		return m_OutputColorCount;
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
		std::string MaterialName = m_MaterialFrameName + "_" + std::to_string(m_CreateCounter);
		Material->SetMaterialName(MaterialName);
		Material->SetOutputColorCount(m_OutputColorCount);

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

		Material->SetMaterialFrame(shared_from_this());
		m_RefMaterialList.push_back(Material);

		return Material;
	}

	bool CMaterialFrame::DeleteRefMaterial(const std::shared_ptr<graphics::CMaterial>& Material)
	{
		const auto& it = std::find(m_RefMaterialList.begin(), m_RefMaterialList.end(), Material);

		if (it == m_RefMaterialList.end()) return false;

		m_RefMaterialList.erase(it);
		m_RefMaterialList.shrink_to_fit();

		return true;
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
			Material->SetOutputColorCount(m_OutputColorCount);

			// APIレベルでマテリアルを更新する
			if (!Material->ReCreate(m_CreateInfo, ShaderBufferList, TextureBindingLayoutList)) return false;
		}

		return true;
	}
}