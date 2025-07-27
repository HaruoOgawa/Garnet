#include "CPostProcess.h"


namespace graphics
{
	CPostProcess::CPostProcess(const std::string& TargetPassName):
		m_UseFXAA(false),
		m_UseBloom(false),
		m_FXAAFilter(std::make_shared<CPostProcessFXAA>(TargetPassName)),
		m_BloomFilter(std::make_shared<CPostProcessBloom>(TargetPassName))
	{
	}

	CPostProcess::~CPostProcess()
	{
	}

	void CPostProcess::SetUseFXAA(bool Flag)
	{
		m_UseFXAA = Flag;
	}

	void CPostProcess::SetUseBloom(bool Flag)
	{
		m_UseBloom = Flag;
	}

	const std::shared_ptr<CPostProcessFXAA>& CPostProcess::GetFXAAFilter() const
	{
		return m_FXAAFilter;
	}

	const std::shared_ptr<CPostProcessBloom>& CPostProcess::GetBloomFilter() const
	{
		return m_BloomFilter;
	}

	bool CPostProcess::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		if (m_UseFXAA)
		{
			if (!m_FXAAFilter->Initialize(pGraphicsAPI, pLoadWorker)) return false;
		}

		if (m_UseBloom)
		{
			if (!m_BloomFilter->Initialize(pGraphicsAPI, pLoadWorker)) return false;
		}

		return true;
	}

	bool CPostProcess::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (m_UseFXAA)
		{
			if (!m_FXAAFilter->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		}

		if (m_UseBloom)
		{
			if (!m_BloomFilter->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		}

		return true;
	}

	bool CPostProcess::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (m_UseFXAA)
		{
			if (!m_FXAAFilter->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
		}

		if (m_UseBloom)
		{
			if (!m_BloomFilter->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}
}