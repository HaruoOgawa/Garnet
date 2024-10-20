#include "CCDIKTest.h"
#include <Object/C3DObject.h>
#include <Scene/CSceneController.h>

namespace component
{
	CCDIKTest::CCDIKTest(const std::string& ComponentName, const std::string& RegistryName):
		CComponent(ComponentName, RegistryName)
	{
	}

	CCDIKTest::~CCDIKTest()
	{
	}

	bool CCDIKTest::OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController,
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		m_NodeList = Object->GetNodeList();

		return true;
	}

	bool CCDIKTest::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		return true;
	}

	bool CCDIKTest::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		return true;
	}
}