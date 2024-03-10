#pragma once

#include <memory>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "../../Interface/IMorphExecutor.h"

namespace graphics
{
	class CVertexBuffer;
	class CPrimitive;

	struct SNodeState
	{
		std::map<int, float> PrevWeightMap;
	};

	class CCPUMorphExecutor : public IMorphExecutor
	{
		float m_WaitTime;
		const float m_MaxWaitTime;

		std::map<int, std::shared_ptr<SNodeState>> m_NodeState;
	private:
		bool ExecuteMorph(std::vector<float>& PosAttribute, const std::map<int, glm::vec3>& MorphVectorMap, float MorphWeight);
		bool ApplyAttribute(const std::vector<float>& PosAttribute, const std::shared_ptr<CPrimitive>& Primitive);
	public:
		CCPUMorphExecutor();
		virtual ~CCPUMorphExecutor();

		virtual bool Create(const std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList) override;

		virtual bool Update(float DeltaSecondsTime, const std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList) override;
	};
}