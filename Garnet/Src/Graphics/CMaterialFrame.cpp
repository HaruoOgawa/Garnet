#include "CMaterialFrame.h"

namespace graphics
{
	CMaterialFrame::CMaterialFrame():
		m_CreateCounter(0),
		m_MaterialFrameName(std::string()),
		m_EnabledZTest(true),
		m_DepthFunc(graphics::EDepthFunc::Less),
		m_StencilParam({}),
		m_CullMode(graphics::ECullMode::NOT_SET),
		m_BlendType(graphics::EBlendType::BLEND_TYPE_NONE),
		m_FileName(std::string()),
		m_CreateInfo(nullptr),
		m_OutputColorCount(1),
		m_RenderQueue(2000)
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

	void CMaterialFrame::SetEnabledZTest(bool ZWrite)
	{
		m_EnabledZTest = ZWrite;
	}

	bool CMaterialFrame::IsEnabledZTest() const
	{
		return m_EnabledZTest;
	}

	void CMaterialFrame::SetDepthFunc(graphics::EDepthFunc DepthFunc)
	{
		m_DepthFunc = DepthFunc;
	}

	graphics::EDepthFunc CMaterialFrame::GetDepthFunc() const
	{
		return m_DepthFunc;
	}

	void CMaterialFrame::SetStencilParam(const SStencilParam& Param)
	{
		m_StencilParam = Param;
	}

	const SStencilParam& CMaterialFrame::GetStencilParam() const
	{
		return m_StencilParam;
	}

	void CMaterialFrame::SetCullMode(graphics::ECullMode CullMode)
	{
		m_CullMode = CullMode;
	}

	graphics::ECullMode CMaterialFrame::GetCullMode() const
	{
		return m_CullMode;
	}

	void CMaterialFrame::SetBlendType(graphics::EBlendType Type)
	{
		m_BlendType = Type;
	}

	graphics::EBlendType CMaterialFrame::GetBlendType() const
	{
		return m_BlendType;
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

	// 描画優先順位
	void CMaterialFrame::SetRenderQueue(int Val)
	{
		m_RenderQueue = Val;
	}

	int CMaterialFrame::GetRenderQueue() const
	{
		return m_RenderQueue;
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

	std::shared_ptr<CMaterial> CMaterialFrame::CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, graphics::ECullMode CullMode)
	{
		if (!m_CreateInfo) return nullptr;

		std::shared_ptr<CMaterial> Material = pGraphicsAPI->CreateMaterial(m_CreateInfo, CullMode);

		// MaterialName
		std::string MaterialName = m_MaterialFrameName + "_" + std::to_string(m_CreateCounter);
		Material->SetMaterialName(MaterialName);
		Material->SetOutputColorCount(m_OutputColorCount);
		Material->SetRenderQueue(m_RenderQueue);

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
			Material->AddTextureBindingLayout(graphics::STextureBindingLayout(
				TextureBuffer.TextureName, TextureBuffer.ViewBindingIndex, TextureBuffer.SamplerBindingIndex, TextureBuffer.
				TextureIndex, TextureBuffer.TextureUsage, TextureBuffer.ReadOnFragment, TextureBuffer.ReadOnVertex
			));
		}

		// その他パラメーター
		Material->SetEnabledZTest(m_EnabledZTest);
		Material->SetDepthFunc(m_DepthFunc);
		Material->SetStencilParam(m_StencilParam);
		Material->SetBlendType(m_BlendType);

		// カウンターを更新
		m_CreateCounter++;

		Material->SetMaterialFrame(shared_from_this());
		m_RefMaterialList.push_back(Material);

		return Material;
	}

	// 全てのマテリアルの値を受け継ぐ
	std::shared_ptr<CMaterial> CMaterialFrame::CreateAndOverriteMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterial>& SrcMaterial)
	{
		auto NewMaterial = CreateMaterial(pGraphicsAPI, SrcMaterial->GetCullMode());

		SetCullMode(SrcMaterial->GetCullMode());

		// UniformValueを設定
		for (const auto& UniformBuffer : SrcMaterial->GetShaderBufferList())
		{
			const auto& BufferData = UniformBuffer->GetBuffer();

			const auto& Descriptor = UniformBuffer->GetDescriptor();

			for (const auto& UniformDataMap : Descriptor.GetDataList())
			{
				const auto& UniformData = UniformDataMap.second;
				const std::string& UniformName = UniformData.UniformName;

				const auto& UniformValue = UniformBuffer->GetUniformValue(UniformName);
				if (UniformValue.empty()) continue;

				NewMaterial->SetUniformValue(UniformName, &UniformValue[0], UniformData.ByteSize);
			}
		}

		// Textureを設定
		for (const auto& TextureBindingLayout : SrcMaterial->GetTextureBindingLayoutList())
		{
			const auto& TextureName = TextureBindingLayout.TextureName;

			int TextureIndex = TextureBindingLayout.TextureIndex;

			if (TextureIndex == -1) continue;

			NewMaterial->ReplaceTextureIndex(TextureName, TextureIndex);
		}

		// TrackIDList
		NewMaterial->SetRefTrackIDList(SrcMaterial->GetRefTrackIDList());

		return NewMaterial;
	}

	std::shared_ptr<CMaterial> CMaterialFrame::CopyMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CMaterial>& SrcMaterial)
	{
		if (!SrcMaterial) return nullptr;

		std::shared_ptr<CMaterial> DstMaterial = CreateMaterial(pGraphicsAPI, SrcMaterial->GetCullMode());

		// ShaderBufferの値をコピー
		if (DstMaterial->GetShaderBufferList().size() != SrcMaterial->GetShaderBufferList().size()) return nullptr;

		for (int i = 0; i < static_cast<int>(DstMaterial->GetShaderBufferList().size()); i++)
		{
			const auto& SrcBuffer = SrcMaterial->GetShaderBufferList()[i];
			const auto& DstBuffer = DstMaterial->GetShaderBufferList()[i];

			DstBuffer->SetBuffer(SrcBuffer->GetBuffer());
		}

		// TextureBufferの値をコピー
		DstMaterial->SetTextureBindingLayoutList(SrcMaterial->GetTextureBindingLayoutList());

		return DstMaterial;
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
			TextureBindingLayoutList.push_back(graphics::STextureBindingLayout(
				TextureBuffer.TextureName, TextureBuffer.ViewBindingIndex, TextureBuffer.SamplerBindingIndex, TextureBuffer.
				TextureIndex, TextureBuffer.TextureUsage, TextureBuffer.ReadOnFragment, TextureBuffer.ReadOnVertex
			));
		}

		//
		for (auto& Material : m_RefMaterialList)
		{
			Material->SetOutputColorCount(m_OutputColorCount);
			Material->SetRenderQueue(m_RenderQueue);

			// APIレベルでマテリアルを更新する
			if (!Material->ReCreate(m_CreateInfo, ShaderBufferList, TextureBindingLayoutList)) return false;
		}

		return true;
	}
}