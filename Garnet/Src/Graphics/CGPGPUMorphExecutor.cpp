#include "CGPGPUMorphExecutor.h"
#include "CMesh.h"
#include "../Object/CNode.h"

namespace graphics
{
	CGPGPUMorphExecutor::CGPGPUMorphExecutor()
	{
	}

	CGPGPUMorphExecutor::~CGPGPUMorphExecutor()
	{
	}

	bool CGPGPUMorphExecutor::Create(const std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		return true;
	}

	bool CGPGPUMorphExecutor::Update(float DeltaSecondsTime, const std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		return true;
	}
}