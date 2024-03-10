#pragma once

#include <vector>
#include <memory>

namespace object{ class CNode; }

namespace graphics
{
	class CMesh;

	class IMorphExecutor
	{
	public:
		virtual bool Create(const std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList) = 0;

		virtual bool Update(float DeltaSecondsTime, const std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList) = 0;
	};
}