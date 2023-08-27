#if defined(USE_GPGPU) && defined(USE_OPENGL)
#include "COpenGLGPGPUHandler.h"
#include "COpenGLMaterial.h"

namespace api
{
	COpenGLGPGPUHandler::COpenGLGPGPUHandler(api::COpenGLAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterial>& ComputeMaterial) :
		m_pGraphicsAPI(pGraphicsAPI),
		m_ComputeMaterial(ComputeMaterial)
	{
	}

	COpenGLGPGPUHandler::~COpenGLGPGPUHandler()
	{
	}

	bool COpenGLGPGPUHandler::Create()
	{
		if (!m_ComputeMaterial) return false;

		if (!m_ComputeMaterial->Create(std::vector<std::shared_ptr<graphics::CTexture>>(), std::vector<std::shared_ptr<graphics::CTexture>>())) return false;

		return true;
	}

	bool COpenGLGPGPUHandler::Dispatch(const glm::ivec3& GroupCount, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		COpenGLMaterial* pSharedOpenGLMat = static_cast<COpenGLMaterial*>(m_ComputeMaterial.get());

		pSharedOpenGLMat->SetActive();

		if (!pSharedOpenGLMat->SetCommonUniform(SecondsTime, Camera, Projection, DrawInfo)) return false;
		if (!pSharedOpenGLMat->BuildDrawBuffer(0)) return false;

		glDispatchCompute(GroupCount.x, GroupCount.y, GroupCount.z);

		return true;
	}

	const std::shared_ptr<graphics::CMaterial>& COpenGLGPGPUHandler::GetComputeMaterial()
	{
		return m_ComputeMaterial;
	}
}

#endif // USE_GPGPU && USE_OPENGL