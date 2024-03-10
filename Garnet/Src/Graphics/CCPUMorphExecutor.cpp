#include "CCPUMorphExecutor.h"
#include "CMesh.h"
#include "../Object/CNode.h"

namespace graphics
{
	CCPUMorphExecutor::CCPUMorphExecutor()
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
		return true;
	}
}