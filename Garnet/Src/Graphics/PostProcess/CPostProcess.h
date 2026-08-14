#pragma once

#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include <Interface/IGraphicsAPI.h>
#include <Scriptable/CValueRegistry.h>
#include "CPostProcessFXAA.h"
#include "CPostProcessToneMapping.h"
#include "CPostProcessBloom.h"

namespace resource { class CLoadWorker; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace input { class CInputState; }
namespace physics { class IPhysicsEngine; }
namespace scene { class CSceneController; }
namespace graphics
{
	class CFrameRenderer;
	class CDrawInfo;
}

namespace graphics
{
	class CPostProcess
	{
		bool m_UseFXAA;
		bool m_UseToneMapping;
		bool m_UseBloom;

		std::shared_ptr<CPostProcessFXAA> m_FXAAFilter;
		std::shared_ptr<CPostProcessToneMapping> m_ToneMappingFilter;
		std::shared_ptr<CPostProcessBloom> m_BloomFilter;

	public:
		CPostProcess(const std::string& TargetPassName);
		virtual ~CPostProcess();

		void SetUseFXAA(bool Flag);
		void SetUseToneMapping(bool Flag);
		void SetUseBloom(bool Flag);

		const std::shared_ptr<CPostProcessFXAA>& GetFXAAFilter() const;
		const std::shared_ptr<CPostProcessToneMapping>& GetToneMappingFilter() const;
		const std::shared_ptr<CPostProcessBloom>& GetBloomFilter() const;

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);

		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState);

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
	};
}


