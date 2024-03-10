#pragma once

#include <vector>
#include <memory>

#include "../Interface/IMorphExecutor.h"

namespace object
{
	class CNode;
}

namespace graphics
{
	enum class EMorphType
	{
		CPU_MORPH,
		GPU_MORPH,
	};

	class CMorphController
	{
		bool m_IsCreated;

		EMorphType m_MorphType;

		std::vector<std::shared_ptr<object::CNode>> m_MorphNodeList;

		std::shared_ptr<IMorphExecutor> m_MorphExecutor;
	public:
		CMorphController();
		virtual ~CMorphController();

		void SetMorphType(EMorphType MorphType);

		void AddMorphNode(const std::shared_ptr<object::CNode>& MorphNode);

		bool Create(const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList);

		bool Update(float DeltaSecondsTime, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList);
	};
}