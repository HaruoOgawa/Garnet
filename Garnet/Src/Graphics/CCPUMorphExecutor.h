#pragma once

#include "../../Interface/IMorphExecutor.h"

namespace graphics
{
	class CCPUMorphExecutor : public IMorphExecutor
	{
	public:
		CCPUMorphExecutor();
		virtual ~CCPUMorphExecutor();

		virtual bool Create(const std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList) override;

		virtual bool Update(float DeltaSecondsTime, const std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList) override;
	};
}