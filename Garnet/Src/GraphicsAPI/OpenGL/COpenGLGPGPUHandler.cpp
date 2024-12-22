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

		if (!m_ComputeMaterial->Create(std::vector<std::string>(), nullptr)) return false;

		return true;
	}

	bool COpenGLGPGPUHandler::Dispatch(const glm::ivec3& GroupCount, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		COpenGLMaterial* pSharedOpenGLMat = static_cast<COpenGLMaterial*>(m_ComputeMaterial.get());

		pSharedOpenGLMat->SetActive();

		pSharedOpenGLMat->SetUniformValue("cameraPos", &Camera->GetPos()[0], sizeof(glm::vec3));
		pSharedOpenGLMat->SetUniformValue("time", &glm::vec1(DrawInfo->GetSecondsTime())[0], sizeof(glm::vec1));
		pSharedOpenGLMat->SetUniformValue("deltaTime", &glm::vec1(DrawInfo->GetDeltaSecondsTime())[0], sizeof(glm::vec1));

		if (!pSharedOpenGLMat->BuildDrawBuffer()) return false;

		glDispatchCompute(GroupCount.x, GroupCount.y, GroupCount.z);

		return true;
	}

	const std::shared_ptr<graphics::CMaterial>& COpenGLGPGPUHandler::GetComputeMaterial()
	{
		return m_ComputeMaterial;
	}
}

#endif // USE_GPGPU && USE_OPENGL