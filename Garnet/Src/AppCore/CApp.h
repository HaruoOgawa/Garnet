#pragma once

#include "../Interface/IGraphicsAPI.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace physics { class IPhysicsEngine; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { class CDrawInfo; }
namespace imageeffect { class CBlurEffect; }
namespace resource { class CLoadWorker; }
namespace input { class CInputState; }
namespace object { class C3DObject; }
namespace scene { class CSceneController; }
namespace gui { class IGUIEngine; }
namespace scriptable { class CComponent; }
namespace api { enum class ERenderPassFormat; }

namespace app
{
	class CApp : public std::enable_shared_from_this<CApp>
	{
	public:
		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker);
		virtual bool Release(api::IGraphicsAPI* pGraphicsAPI);
		virtual bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI);
		virtual bool Resize(int Width, int Height);
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<input::CInputState>& InputState);
		virtual bool LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker);
		virtual bool FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker);
		virtual bool Draw(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<input::CInputState>& InputState,
			const std::shared_ptr<gui::IGUIEngine>& GUIEngine);

		virtual std::shared_ptr<graphics::CDrawInfo> GetDrawInfo() const;

		virtual std::vector<unsigned char> GetEmbeddedBinary(const std::string& Key);

		// コンポーネント作成
		virtual std::shared_ptr<scriptable::CComponent> CreateComponent(const std::string& ComponentType, const std::string& ValueRegistry);

		// 起動準備完了
		virtual bool OnStartup(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine);

		// ロード完了イベント
		virtual bool OnLoaded(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine);

		// フォーカスイベント
		virtual void OnFocus(bool Focused, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);

		// エラー通知イベント
		virtual void OnAssertError(const std::string& Message);

		// Getter
		virtual std::vector<std::shared_ptr<object::C3DObject>> GetObjectList() const;
		virtual std::shared_ptr<scene::CSceneController> GetSceneController() const;

		// タイムライン再生停止イベント
		virtual void OnPlayedTimeline(bool IsPlay);

		// カメラモード変更イベント
		virtual void OnChangeCameraMode(const std::string& Mode);

		// シーン再生モード変更イベント
		virtual void OnChangeScenePlayMode(const std::string& Mode);

		// DMXデータ受信イベント
		virtual void OnReceiveArtNetDMX(unsigned short Net, unsigned short SubNet, unsigned short Universe, const std::vector<unsigned char>& DataBuffer);

		// NDIデータ受信イベント
		virtual void OnReceiveNDIImage(const std::vector<unsigned char>& pixelData, int Width, int Height, api::ERenderPassFormat RenderPassFormat);

		// カスタムイベント発火
		virtual void OnRaisedEvent(const std::string& Type, const std::string& Params);
	};
}