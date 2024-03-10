#include "CMorphController.h"
#include "CCPUMorphExecutor.h"
#include "CGPGPUMorphExecutor.h"

namespace graphics
{
	CMorphController::CMorphController():
		m_IsCreated(false),
		m_MorphType(EMorphType::CPU_MORPH),
		m_MorphExecutor(nullptr)
	{
	}
	CMorphController::~CMorphController()
	{
	}

	void CMorphController::SetMorphType(EMorphType MorphType)
	{
		m_MorphType = MorphType;
	}

	void CMorphController::AddMorphNode(const std::shared_ptr<object::CNode>& MorphNode)
	{
		m_MorphNodeList.push_back(MorphNode);
	}

	bool CMorphController::Create(const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		if (m_IsCreated) return true;

		switch (m_MorphType)
		{
		case graphics::EMorphType::CPU_MORPH:
			m_MorphExecutor = std::make_shared<CCPUMorphExecutor>();
			break;
		case graphics::EMorphType::GPU_MORPH:
			m_MorphExecutor = std::make_shared<CGPGPUMorphExecutor>();
			break;
		default:
			m_MorphExecutor = std::make_shared<CCPUMorphExecutor>();
			break;
		}

		if (!m_MorphExecutor->Create(m_MorphNodeList, MeshList)) return false;

		m_IsCreated = true;

		return true;
	}

	bool CMorphController::Update(float DeltaSecondsTime, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		if (!m_IsCreated) return true;

		if (!m_MorphExecutor->Update(DeltaSecondsTime, m_MorphNodeList, MeshList)) return false;

		return true;
	}
}