#include "CScriptScene.h"
#include "../Object/C3DObject.h"
#include "../File/CFileReader.h"
#include "../Debug/Message/Console.h"
#include "../GLTF/CGLTFImporter.h"
#include <glm/glm.hpp>
#include "../Graphics/CDrawInfo.h"
#include "../Camera/CCamera.h"

namespace scene
{
	CScriptScene::CScriptScene()
	{
	}

	bool CScriptScene::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CScriptScene::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
	}
}