#include "CCPUMorphExecutor.h"
#include "CMesh.h"
#include "CVertexBuffer.h"
#include "../Object/CNode.h"
#include "../Message/Console.h"
#include "../Interface/IRenderer.h"

namespace graphics
{
	CCPUMorphExecutor::CCPUMorphExecutor():
		m_WaitTime(0.0f),
		m_MaxWaitTime(0.1f)
	{
	}

	CCPUMorphExecutor::~CCPUMorphExecutor()
	{
	}

	bool CCPUMorphExecutor::Create(const std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		return true;
	}

	bool CCPUMorphExecutor::Update(float DeltaSecondsTime, const std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		// マイフレームは更新しない
		m_WaitTime += DeltaSecondsTime;

		if (m_WaitTime < m_MaxWaitTime)
		{
			return true;
		}

		m_WaitTime = 0.0f;

		for (int NodeIndex = 0; NodeIndex < NodeList.size(); NodeIndex++)
		{
			const auto& MorphNode = NodeList[NodeIndex];
			
			// MorphWeights
			const auto& MorphWeightList = MorphNode->GetCurrentMorphWeights();
			if (MorphWeightList.size() == 0) continue;

			// Meshを取得
			int MeshIndex = MorphNode->GetMeshIndex();
			if (MeshIndex < 0 || MeshIndex >= MeshList.size()) continue;

			const auto& Mesh = MeshList[MeshIndex];
			const auto& PrimitiveList = Mesh->GetPrimitiveList();
			
			// モーフデータ
			for (const auto& MorphData : Mesh->GetMorphDataList())
			{
				// モーフで更新する頂点バッファを取得する
				int PrimitiveIndex = MorphData.first;
				if (PrimitiveIndex < 0 || PrimitiveIndex >= PrimitiveList.size()) continue;

				const auto& Primitive = PrimitiveList[PrimitiveIndex];

				const auto& VertexBuffer = Primitive->GetVertexBuffer();

				const auto& MorphVectorList = MorphData.second;
				if (MorphVectorList.size() == 0) continue;

				// 一致していないもしくはサイズが0ならモーフを実行せずに正常系としてリターンする
				if (MorphVectorList.size() != MorphWeightList.size()) continue;

				// 頂点アトリビュートの初期位置を取得
				std::vector<float> PosAttribute;

				bool NeedUpdate = false;

				// ToDo: ひとまず0番目に入っている前提で取得するが、頂点アトリビュート名のベクターも用意するといいかもしれない
				PosAttribute = VertexBuffer->GetVertices()[0];

				// 頂点バッファに対してモーフィングを実行する
				for (int MorphIndex = 0; MorphIndex < MorphWeightList.size(); MorphIndex++)
				{
					float Weight = MorphWeightList[MorphIndex];

					// 0の時は更新しない
					constexpr float Epsilon = std::numeric_limits<float>::epsilon();
					if (Weight <= Epsilon) continue;

					const auto& MorphVectorMap = MorphVectorList[MorphIndex];

					if (!ExecuteMorph(PosAttribute, MorphVectorMap, Weight)) return false;

					NeedUpdate = true;
				}
				
				// モーフ結果を頂点バッファに反映する
				if (NeedUpdate)
				{
					if (!ApplyAttribute(PosAttribute, Primitive)) return false;
				}
			}
		}

		return true;
	}

	bool CCPUMorphExecutor::ExecuteMorph(std::vector<float>& PosAttribute, const std::map<int, glm::vec3>& MorphVectorMap, float MorphWeight)
	{
		for (const auto& MorphVector : MorphVectorMap)
		{
			int VertexIndex = MorphVector.first;
			const auto& Offset = MorphVector.second;

			PosAttribute[VertexIndex * 3 + 0] += Offset.x * MorphWeight;
			PosAttribute[VertexIndex * 3 + 1] += Offset.y * MorphWeight;
			PosAttribute[VertexIndex * 3 + 2] += Offset.z * MorphWeight;
		}

		return true;
	}

	bool CCPUMorphExecutor::ApplyAttribute(const std::vector<float>& PosAttribute, const std::shared_ptr<CPrimitive>& Primitive)
	{
		for (const auto& Renderer : Primitive->GetRendererList())
		{
			if (!std::get<0>(Renderer)->UpdateVertexBuffer(PosAttribute, Primitive->GetVertexBuffer())) return false;
		}

		return true;
	}
}