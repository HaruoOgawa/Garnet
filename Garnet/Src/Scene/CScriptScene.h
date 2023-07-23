#pragma once
#include <memory>
#include <vector>

#include "../Interface/IGraphicsAPI.h"

namespace file { class CFileReader; }
namespace object { class C3DObject; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { 
	class CDrawInfo; 
	class CTexture;
}

namespace scene
{
	class CScriptScene
	{
	private:
		bool Load(api::IGraphicsAPI* pGraphicsAPI);
	public:
		CScriptScene();
		virtual ~CScriptScene() = default;

		bool Release(api::IGraphicsAPI* pGraphicsAPI);

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI);
		bool Update(api::IGraphicsAPI* pGraphicsAPI);
		bool Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) ;

		// Tex of FrameBuffer
		void SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture);
	};
}