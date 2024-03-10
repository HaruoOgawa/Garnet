#include "CCPUMorphExecutor.h"
#include "CMesh.h"
#include "CVertexBuffer.h"
#include "../Object/CNode.h"
#include "../Debug/Message/Console.h"

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
		for (int NodeIndex = 0; NodeIndex < NodeList.size(); NodeIndex++)
		{
			m_NodeState.emplace(NodeIndex, std::make_shared<SNodeState>());
		}

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

			auto& NodeState = m_NodeState[NodeIndex];
			
			// MorphWeights
			const auto& MorphWeightList = MorphNode->GetCurrentMorphWeights();

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

				// 一致していなければエラーとする
				if (MorphVectorList.size() != MorphWeightList.size())
				{
					Console::Log("[Error - CPUMorphExecutor] MorphVector size don't match MorphWeight size.\n");

					return false;
				}

				// 頂点アトリビュートの初期位置を取得
				std::vector<float> PosAttribute;

				// ToDo: ひとまず0番目に入っている前提で取得するが、頂点アトリビュート名のベクターも用意するといいかもしれない
				PosAttribute = VertexBuffer->GetVertices()[0];

				// 頂点バッファに対してモーフィングを実行する
				for (int MorphIndex = 0; MorphIndex < MorphWeightList.size(); MorphIndex++)
				{
					float Weight = MorphWeightList[MorphIndex];

					// 0の時は更新しない
					constexpr float Epsilon = std::numeric_limits<float>::epsilon();
					if (Weight <= Epsilon) continue;

					//
					auto morphIt = NodeState->PrevWeightMap.find(MorphIndex);
					if (morphIt == NodeState->PrevWeightMap.end())
					{
						NodeState->PrevWeightMap.emplace(MorphIndex, Weight);
					}
					else
					{
						// 前とウェイトの値が同じならスキップする
						if (morphIt->second == Weight)
						{
							continue;
						}
						else
						{
							// 違うので値を更新する
							NodeState->PrevWeightMap[MorphIndex] = Weight;
						}
					}

					const auto& MorphVectorMap = MorphVectorList[MorphIndex];

					if (!ExecuteMorph(PosAttribute, MorphVectorMap, Weight)) return false;
				}
				
				// モーフ結果を頂点バッファに反映する
				if (!ApplyAttribute(PosAttribute, Primitive)) return false;
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

			PosAttribute[VertexIndex * 3 + 0] = Offset.x * MorphWeight;
			PosAttribute[VertexIndex * 3 + 1] = Offset.y * MorphWeight;
			PosAttribute[VertexIndex * 3 + 2] = Offset.z * MorphWeight;
		}

		return true;
	}

	bool CCPUMorphExecutor::ApplyAttribute(const std::vector<float>& PosAttribute, const std::shared_ptr<CPrimitive>& Primitive)
	{
#ifdef USE_OPENGL
		// OpenGLは1つの頂点バッファを複数のインデックスバッファで使いまわす機能がないので頂点バッファ全てを更新する
		// つまりPrimitiveのRenderer単位で更新する
#else
		const auto& VertexBuffer = Primitive->GetVertexBuffer();
#endif

		return true;
	}
}