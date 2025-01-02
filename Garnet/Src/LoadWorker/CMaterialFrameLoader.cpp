#include "CMaterialFrameLoader.h"
#include "CLoadWorker.h"
#include "CShaderLoader.h"

namespace resource
{
	CMaterialFrameLoader::CMaterialFrameLoader(const std::string& filename, const  std::shared_ptr<graphics::CMaterialFrame>& TargetMaterialFrame):
		CResource(filename, 2),
		m_AnalyseDone(false),
		m_CreateInfo(std::make_shared<graphics::CMaterialCreateInfo>()),
		m_MaterialFrameName(std::string()),
		m_EnabledZWrite(true),
		m_DepthFunc(graphics::EDepthFunc::Less),
		m_StencilParam({}),
		m_CullMode(graphics::ECullMode::NOT_SET),
		m_BlendType(graphics::EBlendType::BLEND_TYPE_ADDITIVE),
		m_OutputColorCount(1)
	{
		m_TargetMaterialFrameSet.emplace(TargetMaterialFrame);

		TargetMaterialFrame->SetFileName(m_FileName);
	}

	CMaterialFrameLoader::~CMaterialFrameLoader()
	{
		Release();
	}

	void CMaterialFrameLoader::Release()
	{
		m_MfJson.clear();
		m_AnalyseDone = false;
		m_MfResourceList.clear();
		m_ShaderFileList.clear();
		m_TextureFileList.clear();
		m_ShaderBufferList.clear();
		m_TextureBufferList.clear();
		m_CreateInfo = nullptr;
	}

	bool CMaterialFrameLoader::Reload(resource::CLoadWorker* pLoadWorker)
	{
		if (!CResource::Reload(pLoadWorker)) return false;
		
		Release();
		m_CreateInfo = std::make_shared<graphics::CMaterialCreateInfo>();

		return true;
	}

	bool CMaterialFrameLoader::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp)
	{
		// マテリアルフレームが持っているリソース一覧を取得する
		if (!m_AnalyseDone)
		{
			if (!AnalyseResourceList(pGraphicsAPI, pLoadWorker)) return false;

			m_AnalyseDone = true;

			return true;
		}

		// マテリアルフレームが持っているリソース一覧を読む
		for (auto& Resource : m_MfResourceList)
		{
			switch (Resource->GetStatus())
			{
			case resource::ELoadStatus::None:
				// ファイルのバイナリが実行ファイルに埋め込まれていないかチェックする
				if (pLoadWorker->FindEmbeddedBinary(pGraphicsAPI, pPhysicsEngine, Resource, pApp)) return true;

				// 通常通りロードする
				if (!Resource->Load()) return false;
				return true;

			case resource::ELoadStatus::Loading:
				if (!Resource->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, pApp)) return false;
				return true;

			case resource::ELoadStatus::Loaded:
			{
				m_MfResourceList.erase(m_MfResourceList.begin());
			}
			return true;

			default:
				break;
			}
		}

		// マテリアルフレームを作成
		if (!CreateMaterialFrame(pGraphicsAPI)) return false;

		// 強制終了はしないが、エラー通知を行う
		if (m_Status == resource::ELoadStatus::AssertError) return true;

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		// リソースマネージャーに登録
		pLoadWorker->GetResourceManager()->AddOnMemoryResource(shared_from_this(), nullptr);

		return true;
	}

	void CMaterialFrameLoader::AddReference(const std::shared_ptr<IResource>& Resource)
	{
		// 自身の時は追加しない
		if (Resource == shared_from_this())
		{
			return;
		}

		CMaterialFrameLoader* pMaterialFrameLoader = static_cast<CMaterialFrameLoader*>(Resource.get());

		if (IsLoaded())
		{
			// ロード済みならデータをすぐに渡す
		}
		else
		{
			for (const auto& Target : pMaterialFrameLoader->GetTargetMaterialFrameSet())
			{
				m_TargetMaterialFrameSet.emplace(Target);

				Target->SetFileName(m_FileName);
			}
		}
	}

	const std::set<std::shared_ptr<graphics::CMaterialFrame>>& CMaterialFrameLoader::GetTargetMaterialFrameSet() const
	{
		return m_TargetMaterialFrameSet;
	}

	bool CMaterialFrameLoader::AnalyseResourceList(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		std::string RawData = std::string();
		RawData.resize(m_File->GetData().size());
		std::memcpy(&RawData[0], &m_File->GetData()[0], m_File->GetData().size());

		m_MfJson = json::parse(RawData.c_str());

		// MaterialName
		const auto MaterialName = m_MfJson.find("name");
		if (MaterialName != m_MfJson.end() && MaterialName->is_string())
		{
			m_MaterialFrameName = MaterialName.value();
		}

		// DepthTest
		{
			const auto zwrite = m_MfJson.find("zwrite");
			if (zwrite != m_MfJson.end() && zwrite->is_boolean())
			{
				m_EnabledZWrite = zwrite.value();
			}

			const auto depthfunc = m_MfJson.find("depthfunc");
			if (depthfunc != m_MfJson.end() && depthfunc->is_string())
			{
				std::string depthfunc_str = depthfunc.value();

				if (depthfunc_str == "never")
				{
					m_DepthFunc = graphics::EDepthFunc::Never;
				}
				else if (depthfunc_str == "less")
				{
					m_DepthFunc = graphics::EDepthFunc::Less;
				}
				else if (depthfunc_str == "lessequal")
				{
					m_DepthFunc = graphics::EDepthFunc::LessEqual;
				}
				else if (depthfunc_str == "greater")
				{
					m_DepthFunc = graphics::EDepthFunc::Greater;
				}
				else if (depthfunc_str == "greaterequal")
				{
					m_DepthFunc = graphics::EDepthFunc::GreaterEqual;
				}
				else if (depthfunc_str == "equal")
				{
					m_DepthFunc = graphics::EDepthFunc::Equal;
				}
				else if (depthfunc_str == "notequal")
				{
					m_DepthFunc = graphics::EDepthFunc::NotEqual;
				}
				else if (depthfunc_str == "always")
				{
					m_DepthFunc = graphics::EDepthFunc::Always;
				}
			}
		}

		// m_StencilParam
		// StencilTest
		const auto stencil = m_MfJson.find("stencil");
		if (stencil != m_MfJson.end() && stencil->is_object())
		{
			//
			bool enabled = false;
			GetBoolean("enabled", enabled, stencil);
			m_StencilParam.Enabled = enabled;

			//
			int value = 0;
			GetInt("value", value, stencil);
			m_StencilParam.RefValue = static_cast<char>(value);

			//
			std::string comp_str = std::string();
			GetString("comp", comp_str, stencil);
			{
				if (comp_str == "never")
				{
					m_StencilParam.Func = graphics::EStencilFunc::Never;
				}
				else if (comp_str == "less")
				{
					m_StencilParam.Func = graphics::EStencilFunc::Less;
				}
				else if (comp_str == "lessequal")
				{
					m_StencilParam.Func = graphics::EStencilFunc::LessEqual;
				}
				else if (comp_str == "greater")
				{
					m_StencilParam.Func = graphics::EStencilFunc::Greater;
				}
				else if (comp_str == "greaterequal")
				{
					m_StencilParam.Func = graphics::EStencilFunc::GreaterEqual;
				}
				else if (comp_str == "equal")
				{
					m_StencilParam.Func = graphics::EStencilFunc::Equal;
				}
				else if (comp_str == "notequal")
				{
					m_StencilParam.Func = graphics::EStencilFunc::NotEqual;
				}
				else if (comp_str == "always")
				{
					m_StencilParam.Func = graphics::EStencilFunc::Always;
				}
			}

			//
			std::string pass_str = std::string();
			GetString("pass", pass_str, stencil);
			{
				if (pass_str == "keep")
				{
					m_StencilParam.DpPass = graphics::EStencilOp::Keep;
				}
				else if (pass_str == "replace")
				{
					m_StencilParam.DpPass = graphics::EStencilOp::Replace;
				}
				else if (pass_str == "incr")
				{
					m_StencilParam.DpPass = graphics::EStencilOp::Incr;
				}
				else if (pass_str == "decr")
				{
					m_StencilParam.DpPass = graphics::EStencilOp::Decr;
				}
			}
		}

		// Cull Mode
		const auto cull = m_MfJson.find("cull");
		if (cull != m_MfJson.end() && cull->is_string())
		{
			std::string cull_str = cull.value();

			if (cull_str == "none")
			{
				m_CullMode = graphics::ECullMode::CULL_NONE;
			}
			else if (cull_str == "back")
			{
				m_CullMode = graphics::ECullMode::CULL_BACK;
			}
			else if (cull_str == "front")
			{
				m_CullMode = graphics::ECullMode::CULL_FRONT;
			}
			else
			{
				m_CullMode = graphics::ECullMode::NOT_SET;
			}
		}

		// BlendType
		const auto blendtype = m_MfJson.find("blendtype");
		if (blendtype != m_MfJson.end() && blendtype->is_string())
		{
			std::string blendtype_str = blendtype.value();

			if (blendtype_str == "additive")
			{
				m_BlendType = graphics::EBlendType::BLEND_TYPE_ADDITIVE;
			}
			else if (blendtype_str == "transparent")
			{
				m_BlendType = graphics::EBlendType::BLEND_TYPE_TRANSPARENT_ALPHA;
			}
		}

		// outcolorcount
		const auto outputcolorcount = m_MfJson.find("outputcolorcount");
		if (outputcolorcount != m_MfJson.end() && outputcolorcount->is_number_integer())
		{
			m_OutputColorCount = outputcolorcount.value();
		}

		// shaderList
		const auto shaderList = m_MfJson.find("shaderList");
		if (shaderList != m_MfJson.end() && shaderList->is_array())
		{
			if (!AnalyseShaderList(pGraphicsAPI, pLoadWorker, shaderList)) return false;
		}
		
		// textureList
		const auto textureList = m_MfJson.find("textureList");
		if (textureList != m_MfJson.end() && textureList->is_array())
		{
			if (!AnalyseTextureList(pGraphicsAPI, textureList)) return false;
		}

		return true;
	}

	bool CMaterialFrameLoader::AnalyseShaderList(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const json::iterator& shaderList)
	{
		for (json::iterator shader = shaderList->begin(); shader != shaderList->end(); ++shader)
		{
			if (shader->is_object())
			{
				// name
				std::string shaderName = "";
				GetString("name", shaderName, shader);

				// shaderFile
				std::string shaderFile = "";
				GetString("shaderFile", shaderFile, shader);

				// autoShaderExtension
				bool autoShaderExtension = true;
				GetBoolean("autoShaderExtension", autoShaderExtension, shader);

				// shadercode
				std::string shadercode = "";
				GetString("shadercode", shadercode, shader);

				// shaderType
				std::string shaderType = "";
				GetString("shaderType", shaderType, shader);

				if (!shadercode.empty())
				{
					std::vector<unsigned char> ShaderCodeArray;
					ShaderCodeArray.resize(shadercode.length());
					std::memcpy(&ShaderCodeArray[0], &shadercode[0], shadercode.size());

					// ShaderCodeが直接書かれているのでそのままCreateInfoに渡す
					if (shaderType == "vertex")
					{
						m_CreateInfo->SetVertexShaderCode(ShaderCodeArray);
					}
					else if (shaderType == "fragment")
					{
						m_CreateInfo->SetFragmentShaderCode(ShaderCodeArray);
					}
					else if (shaderType == "compute")
					{
						m_CreateInfo->SetComputeShaderCode(ShaderCodeArray);
					}
					else if (shaderType == "geometry")
					{
						m_CreateInfo->SetGeometryShaderCode(ShaderCodeArray);
					}
					else if (shaderType == "hull")
					{
						m_CreateInfo->SetHullShaderCode(ShaderCodeArray);
					}
					else if (shaderType == "domain")
					{
						m_CreateInfo->SetDomainShaderCode(ShaderCodeArray);
					}
				}
				else
				{
					// ロードファイルリストに入れてファイルをロードする
					std::string fullshaderFilePath = shaderFile;

					std::string EditingBaseFileName = shaderFile;

					// OpenGL・Vulkan・WebGPUに沿った拡張子を自動でつける
					// ファイル名のルールに従う必要がある
					if (autoShaderExtension)
					{
						if (shaderType == "vertex")
						{
							fullshaderFilePath += pGraphicsAPI->GetVertexShaderExtension();
							EditingBaseFileName += ".vert";
						}
						else if (shaderType == "fragment")
						{
							fullshaderFilePath += pGraphicsAPI->GetFragmentShaderExtension();
							EditingBaseFileName += ".frag";
						}
						else if (shaderType == "compute")
						{
							fullshaderFilePath += pGraphicsAPI->GetComputeShaderExtension();
							EditingBaseFileName += ".comp";
						}
						else if (shaderType == "geometry")
						{
							fullshaderFilePath += pGraphicsAPI->GetGeometryShaderExtension();
							EditingBaseFileName += ".geom";
						}
						else if (shaderType == "hull")
						{
							fullshaderFilePath += pGraphicsAPI->GetHullShaderExtension();
							EditingBaseFileName += ".tesc";
						}
						else if (shaderType == "domain")
						{
							fullshaderFilePath += pGraphicsAPI->GetDomainShaderExtension();
							EditingBaseFileName += ".tese";
						}
					}

					// リソースマネージャーに既に登録されていてかつロード済みかチェックする
					auto Resource = pLoadWorker->GetResourceManager()->FindResource(EditingBaseFileName);

					if (Resource)
					{
						// 登録済み
						// 参照の追加
						static_cast<CShaderLoader*>(Resource.get())->AddRefMFLoader(shared_from_this());

						m_ShaderFileList.emplace(shaderType, static_cast<CShaderLoader*>(Resource.get()));

						if (!Resource->IsLoaded())
						{
							// 未ロードなのでロードリストに追加
							m_MfResourceList.push_back(Resource);
						}
					}
					else
					{
						// 未登録なのでリソースマネージャーに登録. ロードリストにも追加
						std::shared_ptr<CShaderLoader> ShaderLoader = std::make_shared<CShaderLoader>(fullshaderFilePath, EditingBaseFileName);

						// 参照の追加
						ShaderLoader->AddRefMFLoader(shared_from_this());

						pLoadWorker->GetResourceManager()->AddOnMemoryResource(ShaderLoader, shared_from_this());

						m_MfResourceList.push_back(ShaderLoader);

						m_ShaderFileList.emplace(shaderType, ShaderLoader);
					}
				}

				// uniformBlockList
				const auto uniformBlockList = shader->find("uniformBlockList");
				for (json::iterator uniform = uniformBlockList->begin(); uniform != uniformBlockList->end(); uniform++)
				{
					std::string uniform_type = "";
					GetString("type", uniform_type, uniform);

					if (uniform_type == "ubo" || uniform_type == "ssbo")
					{
						if (!AnalyseShaderBuffer(uniform, uniform_type)) return false;
					}
					else if (uniform_type == "texture")
					{
						if (!AnalyseTextureBuffer(uniform)) return false;
					}
				}
			}
		}

		return true;
	}

	bool CMaterialFrameLoader::AnalyseShaderBuffer(const json::iterator& uniform, const std::string& uniform_type)
	{
		// 取得
		std::string name = "";
		GetString("name", name, uniform);

		int binding = -1;
		GetInt("binding", binding, uniform);

		bool isWritable = false;
		GetBoolean("isWritable", isWritable, uniform);

		// BindingLayoutを作成
		graphics::SBindingLayout BindingLayout = { name , binding, isWritable };

		std::string bufferUpdateType = "";
		GetString("bufferUpdateType", bufferUpdateType, uniform);

		if (bufferUpdateType.empty() || bufferUpdateType == "cpu")
		{
			BindingLayout.BufferUpdateType = graphics::EBufferUpdateType::UPDATE_TYPE_CPU;
		}
		else if (bufferUpdateType == "gpu")
		{
			BindingLayout.BufferUpdateType = graphics::EBufferUpdateType::UPDATE_TYPE_GPU;
		}

		// BufferValueを取得
		std::vector<std::shared_ptr<graphics::SBufferValueLayout>> ValueLayoutList;

		const auto values = uniform->find("values");
		if (values != uniform->end() && values->is_array())
		{
			for (json::iterator val = values->begin(); val != values->end(); val++)
			{
				std::string value_name = "";
				GetString("name", value_name, val);

				std::string value_type = "";
				GetString("type", value_type, val);

				// ひとまずfloatArrayにしておく. floatもintもバイトサイズは同じなので
				std::vector<float> initValue;
				GetArray32("initValue", initValue, val);

				bool useInitValue = true;
				GetBoolean("useInitValue", useInitValue, val);

				int arraySize = 0;
				GetInt("arraySize", arraySize, val);

				int ByteSize = 0;
				graphics::EUniformValueType ValueType = graphics::EUniformValueType::NONE;

				if (value_type == "mat4")
				{
					ByteSize = sizeof(glm::mat4);
					ValueType = graphics::EUniformValueType::VALUE_TYPE_MAT4;
				}
				else if (value_type == "mat3")
				{
					ByteSize = sizeof(glm::mat3);
					ValueType = graphics::EUniformValueType::VALUE_TYPE_MAT3;
				}
				else if (value_type == "mat2")
				{
					ByteSize = sizeof(glm::mat2);
					ValueType = graphics::EUniformValueType::VALUE_TYPE_MAT2;
				}
				else if (value_type == "vec4")
				{
					ByteSize = sizeof(glm::vec4);
					ValueType = graphics::EUniformValueType::VALUE_TYPE_VEC4;
				}
				else if (value_type == "vec3")
				{
					ByteSize = sizeof(glm::vec3);
					ValueType = graphics::EUniformValueType::VALUE_TYPE_VEC3;
				}
				else if (value_type == "vec2")
				{
					ByteSize = sizeof(glm::vec2);
					ValueType = graphics::EUniformValueType::VALUE_TYPE_VEC2;
				}
				else if (value_type == "float")
				{
					ByteSize = sizeof(float);
					ValueType = graphics::EUniformValueType::VALUE_TYPE_FLOAT;
				}
				else if (value_type == "int")
				{
					ByteSize = sizeof(int);
					ValueType = graphics::EUniformValueType::VALUE_TYPE_INT;
				}
				else if (value_type == "float_array")
				{
					if (useInitValue)
					{
						ByteSize = sizeof(float) * static_cast<int>(initValue.size());
					}
					else
					{
						ByteSize = sizeof(float) * arraySize;

						initValue.resize(arraySize, 0.0f);
					}

					ValueType = graphics::EUniformValueType::VALUE_TYPE_FLOAT_ARRAY;
				}
				else if (value_type == "mat4_array")
				{
					if (useInitValue)
					{
						ByteSize = sizeof(float) * static_cast<int>(initValue.size());
					}
					else
					{
						ByteSize = sizeof(float) * 16 * arraySize;

						initValue.resize(16 * arraySize, 0.0f);
					}

					ValueType = graphics::EUniformValueType::VALUE_TYPE_MAT4_ARRAY;
				}

				// ValueInput
				graphics::SUniformValueInput ValueInput = {};

				const auto input = val->find("input");
				if (input != val->end() && input->is_object())
				{
					// type
					std::string type = "";
					GetString("type", type, input);

					if (type == "range")
					{
						ValueInput.Type = graphics::EUniformInputType::Range;
					}
					else if (type == "color")
					{
						ValueInput.Type = graphics::EUniformInputType::Color;
					}
					else
					{
						ValueInput.Type = graphics::EUniformInputType::None;
					}

					// minvalue, maxvalue
					GetFloat("minvalue", ValueInput.MinValue, input);
					GetFloat("maxvalue", ValueInput.MaxValue, input);

					// hide
					GetBoolean("hide", ValueInput.Hide, input);
				}

				std::shared_ptr<graphics::SBufferValueLayout> ValueLayout = std::make_shared<graphics::SBufferValueLayout>();
				ValueLayout->Name = value_name;
				ValueLayout->ValueType = ValueType;
				ValueLayout->Data = initValue;
				ValueLayout->ByteSize = ByteSize;
				ValueLayout->BindingIndex = binding;
				ValueLayout->ValueInput = ValueInput;

				ValueLayoutList.push_back(ValueLayout);
			}
		}

		if (uniform_type == "ubo")
		{
			m_ShaderBufferList.push_back({ graphics::EBufferType::UNIFORM, BindingLayout, ValueLayoutList });
		}
		else if (uniform_type == "ssbo")
		{
			m_ShaderBufferList.push_back({ graphics::EBufferType::SHADERSTORAGE, BindingLayout, ValueLayoutList });
		}

		return true;
	}

	bool CMaterialFrameLoader::AnalyseTextureBuffer(const json::iterator& uniform)
	{
		std::string name = "";
		GetString("name", name, uniform);

		int viewBinding = -1;
		GetInt("viewBinding", viewBinding, uniform);

		int samplerBinding = -1;
		GetInt("samplerBinding", samplerBinding, uniform);

		int textureIndex = -1;
		GetInt("textureIndex", textureIndex, uniform);

		std::string type = "";
		GetString("type", type, uniform);

		std::string textureUsage_str = "";
		GetString("textureUsage", textureUsage_str, uniform);

		graphics::ETextureUsage TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_2D;
		
		if (textureUsage_str == "2d")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_2D;
		}
		else if (textureUsage_str == "cube")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_CUBE;
		}
		else if (textureUsage_str == "frame")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_FRAME;
		}
		else if (textureUsage_str == "diffuse")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_IBL_Diffuse;
		}
		else if (textureUsage_str == "specular")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_IBL_Specular;
		}
		else if (textureUsage_str == "ggx")
		{
			TextureUsage = graphics::ETextureUsage::TEXTURE_USAGE_IBL_GGXLUT;
		}

		// TextureBindingLayoutを作成
		graphics::STextureBindingLayout BindingLayout = { name , viewBinding, samplerBinding, textureIndex,TextureUsage };

		// 配列に登録
		m_TextureBufferList.push_back(BindingLayout);

		return true;
	}

	bool CMaterialFrameLoader::AnalyseTextureList(api::IGraphicsAPI* pGraphicsAPI, const json::iterator& textureList)
	{
		// あまりメリットがないので未実装
		// C3DObjectのTetureListは他のマテリアルでも共有しており、結局ReplaceTextureIndexでインデックスを調整することになるので無駄である

		return true;
	}

	bool CMaterialFrameLoader::CreateMaterialFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		// ロードが必要だったShaderリストを登録する
		for (const auto& ShaderFile : m_ShaderFileList)
		{
			const auto& shaderType = ShaderFile.first;

			//
			graphics::EShaderStage ShaderStage = graphics::EShaderStage::SHADER_STAGE_NONE;
			if (shaderType == "vertex")
			{
				ShaderStage = graphics::EShaderStage::SHADER_STAGE_VERTEX;
			}
			else if (shaderType == "fragment")
			{
				ShaderStage = graphics::EShaderStage::SHADER_STAGE_FRAGMENT;
			}
			else if (shaderType == "compute")
			{
				ShaderStage = graphics::EShaderStage::SHADER_STAGE_COMPUTE;
			}
			else if (shaderType == "geometry")
			{
				ShaderStage = graphics::EShaderStage::SHADER_STAGE_GEOMETRY;
			}
			else if (shaderType == "hull")
			{
				ShaderStage = graphics::EShaderStage::SHADER_STAGE_HULL;
			}
			else if (shaderType == "domain")
			{
				ShaderStage = graphics::EShaderStage::SHADER_STAGE_DOMAIN;
			}

			//
			std::vector<unsigned char> ShaderCodeArray = ShaderFile.second->GetFile()->GetData();

			// コンパイルエラーチェック
			std::string ErrorMsg = std::string();

			// リロードでかつコンパイルエラーなら通知する
			// リロードでなければ通常通りShaderCompileエラーで止める
			if(m_Releoading &&!pGraphicsAPI->CheckValidShader(ErrorMsg, ShaderCodeArray, ShaderStage))
			{
				// コンパイルエラーが出たので終了する
				m_AssertedErrorMessage = "[ShaderCompileError] " + ShaderFile.second->GetFile()->GetFilename() + " : " + ErrorMsg;
				m_Status = resource::ELoadStatus::AssertError;

				return true;
			}

			// ShaderCodeが直接書かれているのでそのままCreateInfoに渡す
			if (ShaderStage == graphics::EShaderStage::SHADER_STAGE_VERTEX)
			{
				m_CreateInfo->SetVertexShaderCode(ShaderCodeArray);
			}
			else if (ShaderStage == graphics::EShaderStage::SHADER_STAGE_FRAGMENT)
			{
				m_CreateInfo->SetFragmentShaderCode(ShaderCodeArray);
			}
			else if (ShaderStage == graphics::EShaderStage::SHADER_STAGE_COMPUTE)
			{
				m_CreateInfo->SetComputeShaderCode(ShaderCodeArray);
			}
			else if (ShaderStage == graphics::EShaderStage::SHADER_STAGE_GEOMETRY)
			{
				m_CreateInfo->SetGeometryShaderCode(ShaderCodeArray);
			}
			else if (ShaderStage == graphics::EShaderStage::SHADER_STAGE_HULL)
			{
				m_CreateInfo->SetHullShaderCode(ShaderCodeArray);
			}
			else if (ShaderStage == graphics::EShaderStage::SHADER_STAGE_DOMAIN)
			{
				m_CreateInfo->SetDomainShaderCode(ShaderCodeArray);
			}
		}

		m_ShaderFileList.clear();

		// ロードが必要だったテクスチャリストを登録する

		// MaterialFrameを生成
		for (auto& MaterialFrame : m_TargetMaterialFrameSet)
		{
			if (MaterialFrame)
			{
				MaterialFrame->SetMaterialFrameName(m_MaterialFrameName);
				MaterialFrame->SetEnabledZWrite(m_EnabledZWrite);
				MaterialFrame->SetDepthFunc(m_DepthFunc);
				MaterialFrame->SetStencilParam(m_StencilParam);
				MaterialFrame->SetCullMode(m_CullMode);
				MaterialFrame->SetBlendType(m_BlendType);
				MaterialFrame->SetCreateInfo(m_CreateInfo);
				MaterialFrame->SetShaderBufferList(m_ShaderBufferList);
				MaterialFrame->SetTextureBufferList(m_TextureBufferList);
				MaterialFrame->SetOutputColorCount(m_OutputColorCount);

				// リロードなのでLoaderを参照しているマテリアルフレームにも更新を実行する
				if (m_Releoading)
				{
					if (!MaterialFrame->Reload()) return false;
				}
			}
		}

		return true;
	}

	void CMaterialFrameLoader::GetString(const std::string& Key, std::string& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_string())
		{
			Value = it.value();
		}
	}

	void CMaterialFrameLoader::GetBoolean(const std::string& Key, bool& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_boolean())
		{
			Value = it.value();
		}
	}

	void CMaterialFrameLoader::GetInt(const std::string& Key, int& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_number_integer())
		{
			Value = it.value();
		}
	}

	void CMaterialFrameLoader::GetFloat(const std::string& Key, float& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_number_float())
		{
			Value = it.value();
		}
	}

	void CMaterialFrameLoader::GetArray32(const std::string& Key, std::vector<float>& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_array())
		{
			for (json::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				if (it2->is_number())
				{
					float val = it2.value();

					Value.push_back(val);
				}
			}
		}
	}
}