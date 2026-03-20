#include "CMTLImporter.h"
#include "../../Interface/IGraphicsAPI.h"
#include "../../Object/C3DObject.h"
#include "../../Graphics/CMaterialFrame.h"
#include "../../LoadWorker/C3DObjectLoader.h"
#include "../../Binary/CBinaryReader.h"
#include "../../Format/CStringFormatter.h"
#ifdef _DEBUG
#include "../../Message/Console.h"
#endif // _DEBUG
#include "../../LoadWorker/CTextureLoader.h"

namespace obj
{
	bool CMTLImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data,
		const std::string& BaseDir, const std::shared_ptr<object::C3DObject>& Object,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList,
		resource::C3DObjectLoader* p3DObjectLoader)
	{
		binary::CBinaryReader Analyzer(Data);

		std::vector<std::string> LineList;

		std::string CurrentMeshName = std::string();

		std::map<std::string, std::vector<std::shared_ptr<graphics::CMaterial>>> MaterialMap;

		for (const auto& Mesh : Object->GetMeshList())
		{
			for (const auto& Primitive : Mesh->GetPrimitiveList())
			{
				for (const auto& Renderer : Primitive->GetRendererList())
				{
					const auto& Material = std::get<1>(Renderer);
					if (!Material) continue;

					MaterialMap[Material->GetMaterialName()].push_back(Material);
				}
			}
		}

		std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
		std::map<std::string, int> TextureIndexMap;

		for (;;)
		{
			if (Analyzer.IsEnd()) break;

			std::string CurrentLine = std::string();
			if (!Analyzer.GetStringToLineBreak(CurrentLine)) return false;
			LineList.push_back(CurrentLine);

			std::vector<std::string> ParamList = format::CStringFormatter::Sprit(CurrentLine, ' ');
			if (ParamList.empty()) continue;

			// タブやスペースなどの空白を除去する
			EraseSpaceFromParamList(ParamList);

			Analyze(pGraphicsAPI, ParamList, CurrentLine, BaseDir, Object, p3DObjectLoader, CurrentMeshName, MaterialMap, TextureList, TextureIndexMap);
		}

		for (const auto& Texture : TextureList)
		{
			Object->GetTextureSet()->Add2DTexture(Texture);
		}

		return true;
	}

	void CMTLImporter::Analyze(api::IGraphicsAPI* pGraphicsAPI,
		const std::vector<std::string>& ParamList, const std::string& CurrentLine, const std::string& BaseDir,
		const std::shared_ptr<object::C3DObject>& Object, resource::C3DObjectLoader* p3DObjectLoader, std::string& CurrentMeshName,
		std::map<std::string, std::vector<std::shared_ptr<graphics::CMaterial>>>& MaterialMap,
		std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, std::map<std::string, int>& TextureIndexMap)
	{
		if (ParamList[0] == "newmtl" && ParamList.size() >= 2)
		{
			CurrentMeshName = ParamList[1];
		}
		else if (ParamList[0] == "Ka" && ParamList.size() >= 4)
		{
			// アンビエントカラー
			SetMaterialUniform(CurrentMeshName, "ambientColor", 
				{ std::stof(ParamList[1]), std::stof(ParamList[2]), std::stof(ParamList[3]), 1.0f }, MaterialMap);
		}
		else if (ParamList[0] == "Kd" && ParamList.size() >= 4)
		{
			// ディフューズカラー(拡散反射色)
			SetMaterialUniform(CurrentMeshName, "baseColorFactor",
				{ std::stof(ParamList[1]), std::stof(ParamList[2]), std::stof(ParamList[3]), 1.0f }, MaterialMap);
		}
		else if (ParamList[0] == "Ks" && ParamList.size() >= 4)
		{
			// スペキュラーカラー(鏡面反射色)
		}
		else if (ParamList[0] == "Ke" && ParamList.size() >= 4)
		{
			// エミッシブカラー
			SetMaterialUniform(CurrentMeshName, "emissiveFactor",
				{ std::stof(ParamList[1]), std::stof(ParamList[2]), std::stof(ParamList[3]), 1.0f }, MaterialMap);
		}
		else if (ParamList[0] == "Ns" && ParamList.size() >= 2)
		{
			// スペキュラーハイライトの強さ(Phong Exponent)
			// これをPBRのRoughnessに変換する
			// Exponent : 値が大きいほどハイライトが強くなる。つまり滑らかになる
			// Roughness : 値が大きいほどハイライトが弱くなる。つまり表面が荒くなる。
			// これらは近似的に「 Roughness = sqrt(2 / (Exponent + 2)) 」で求まる
			// https://graphicscompendium.com/gamedev/15-pbr
			float Exponent = std::stof(ParamList[1]);

			float Roughness = glm::sqrt(2.0f / (Exponent + 2.0f));
			SetMaterialUniform(CurrentMeshName, "roughnessFactor", { Roughness }, MaterialMap);

			// メタリックは常に0とする
			SetMaterialUniform(CurrentMeshName, "metallicFactor", { 0.0f }, MaterialMap);
		}
		else if (ParamList[0] == "Ni" && ParamList.size() >= 2)
		{
			// 屈折率
		}
		else if (ParamList[0] == "d" && ParamList.size() >= 2)
		{
			// 不透明度
		}
		else if (ParamList[0] == "Tr" && ParamList.size() >= 2)
		{
			// 透明度
		}
		else if (ParamList[0] == "Tf" && ParamList.size() >= 4)
		{
		}
		else if (ParamList[0] == "illum" && ParamList.size() >= 2)
		{
			// どのライティングモデルを使うか
			/*
				0 = ライティングなし（色のみ）
				1 = 拡散反射のみ
				2 = 拡散 + 鏡面反射（一般的）
			*/
		}
		else if (ParamList[0] == "map_Ka" && ParamList.size() >= 2)
		{
			// アンビエントマップ
		}
		else if (ParamList[0] == "map_Kd" && ParamList.size() >= 2)
		{
			// ディフューズマップ
			SetTextureUniform(pGraphicsAPI, BaseDir, p3DObjectLoader, CurrentMeshName, "baseColorTexture", ParamList[1], MaterialMap, TextureList, TextureIndexMap);
		}
		else if (ParamList[0] == "map_bump" && ParamList.size() >= 2)
		{
			// バンプマップ
			SetTextureUniform(pGraphicsAPI, BaseDir, p3DObjectLoader, CurrentMeshName, "normalTexture", ParamList[1], MaterialMap, TextureList, TextureIndexMap);
		}
		else if (ParamList[0] == "#")
		{
			// コメントなので何もしない
			return;
		}
		else
		{
#ifdef _DEBUG
			Console::Log("[COBJImporter::Analyze] Non Supported Type Found. (%s)\n", CurrentLine.c_str());
#endif // _DEBUG
		}
	}

	void CMTLImporter::EraseSpaceFromParamList(std::vector<std::string>& ParamList)
	{
		for (auto& Param : ParamList)
		{
			std::string NewStr = std::string();

			for (auto val = Param.begin(); val != Param.end(); val++)
			{
				char c = *val;

				if (IsSkipChara(c)) continue;

				NewStr += c;
			}

			Param = NewStr;
		}
	}

	bool CMTLImporter::IsSkipChara(char c)
	{
		switch (c)
		{
		case '\n': // 0x0a
		case '\t': // 0x09
		case '\r': // 0x0d
		case 0x20: // 半角スペース
			return true;
		default:
			break;
		}

		return false;
	}

	void CMTLImporter::SetMaterialUniform(const std::string& CurrentMeshName, const std::string& UniformName, const std::vector<float> Values,
		std::map<std::string, std::vector<std::shared_ptr<graphics::CMaterial>>>& MaterialMap)
	{
		const auto& it = MaterialMap.find(CurrentMeshName);
		if (it != MaterialMap.end())
		{
			const int Dimention = static_cast<int>(Values.size());

			auto& MaterialList = it->second;

			for (auto& Material : MaterialList)
			{
				Material->ReplacePreloadUniformValue(UniformName, &Values[0], sizeof(float) * Dimention, 0);
			}
		}
	}

	void CMTLImporter::SetTextureUniform(api::IGraphicsAPI* pGraphicsAPI, const std::string& BaseDir,
		resource::C3DObjectLoader* p3DObjectLoader,
		const std::string& CurrentMeshName, const std::string& UniformName, const std::string& TextureName,
		std::map<std::string, std::vector<std::shared_ptr<graphics::CMaterial>>>& MaterialMap,
		std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, std::map<std::string, int>& TextureIndexMap)
	{
		int TextureIndex = -1;
		const auto& it = TextureIndexMap.find(TextureName);

		if (it == TextureIndexMap.end())
		{
			// 未登録テクスチャなので新規生成する
			TextureIndex = static_cast<int>(TextureList.size());
			TextureIndexMap[TextureName] = TextureIndex;

			// ロードする
			std::string FullPath = BaseDir + "/" + TextureName;

			std::shared_ptr<graphics::CTexture> Texture = pGraphicsAPI->CreateTexture();
			std::shared_ptr<resource::CTextureLoader> TexLoader = std::make_shared<resource::CTextureLoader>(pGraphicsAPI, FullPath, Texture);

			TextureList.push_back(Texture);
			p3DObjectLoader->AddSubResource(TexLoader);
		}
		else
		{
			TextureIndex = it->second;
		}

		// マテリアルにテクスチャインデックスを設定
		if (TextureIndex != -1)
		{
			if (!UniformName.empty())
			{
				const auto& it = MaterialMap.find(CurrentMeshName);
				if (it != MaterialMap.end())
				{
					auto& MaterialList = it->second;

					for (auto& Material : MaterialList)
					{
						Material->ReplaceTextureIndex(UniformName, TextureIndex);
					}
				}
			}
		}
	}
}