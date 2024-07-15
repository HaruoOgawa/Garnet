#pragma once

#include <memory>
#include <vector>
#include <map>
#include <tuple>
#include <string>

#include <Interface/IGraphicsAPI.h>
#include <Interface/IPhysicsEngine.h>
#include <Graphics/ECullMode.h>

namespace object { class C3DObject; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace resource { class CLoadWorker; }
namespace input { class CInputState; }
namespace graphics { 
	class CDrawInfo; 
	class CMaterialFrame;
	class CTexture;
	class CTextureSet;
}
namespace animation { class CAnimationClipSet; }
namespace audio { class CAudioClip; }

namespace scene
{
	struct SUniformInfo
	{
		std::string UniformName = "";
		int ByteSize = 0;
		std::vector<unsigned char> UniformData;
	};

	struct SMaterialInfo 
	{
		std::string MaterialFrameName = "";
		std::vector<SUniformInfo> UniformInfoList;
		std::vector<std::tuple<std::string, std::string, int>> Textures;
		int RefCount = 0;
		graphics::ECullMode CullMode = graphics::ECullMode::CULL_BACK;
		std::vector<std::string> TrackIDList;
	};

	struct SAnimationClip
	{
	};

	struct SHumanoidclip
	{
		std::string Key = "";
		std::string MotionName = "";
		int Index = -1;
		bool Loop = false;
		bool IK = false;
	};

	struct SBlendshape
	{
		std::string Key = "";
		std::string MotionName = "";
		int Index = -1;
		bool Loop = false;
	};

	struct SAnimationInfo
	{
		std::vector<SAnimationClip> Clips;
		std::map<std::string, SHumanoidclip> Humanoidclips;
		std::map<std::string, SBlendshape> Blendshapes;

		std::string PlayMotion;
		int PlayMotionIndex = -1;
		std::vector<std::string> PlayBlendShapes;
	};

	class CSceneController
	{
		std::string m_FileName;

		//
		std::vector<std::shared_ptr<object::C3DObject>> m_ObjectList;

		//
		std::map<std::string, std::shared_ptr<graphics::CMaterialFrame>> m_MaterialFrameMap;
		std::map<std::string, std::shared_ptr<animation::CAnimationClipSet>> m_AnimationClipSetMap;
		std::shared_ptr<graphics::CTextureSet> m_SceneTextureSet;
		std::tuple<std::shared_ptr<audio::CAudioClip>, bool, bool> m_BGM;

		//
		std::map<std::shared_ptr<object::C3DObject>, std::vector<SMaterialInfo>> m_MaterialInfoMap;
		std::map<std::shared_ptr<object::C3DObject>, std::map<std::string, std::shared_ptr<graphics::CTexture>>> m_TextureInfoMap;
		std::map<std::shared_ptr<object::C3DObject>, SAnimationInfo> m_AnimationInfoMap;
		
	private:
		bool PrepareTextureList(const std::shared_ptr<object::C3DObject>& Object, std::map<std::string, int>& TexIndexMap);
		bool PrepareMaterialList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, int>& TexIndexMap);
		bool PrepareAnimationList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object);
		bool CreateMaterialList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, int>& TexIndexMap);
		bool UpdateMaterialUniform(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, int>& TexIndexMap);
	public:
		CSceneController();
		virtual ~CSceneController();

		void SetFileName(const std::string& Name);
		const std::string& GetFileName() const;

		//
		void AddObject(const std::shared_ptr<object::C3DObject>& Object);
		std::vector<std::shared_ptr<object::C3DObject>> GetObjectList() const;
		std::shared_ptr<object::C3DObject> FindObjectByName(const std::string& Name);

		void AddMaterialFrame(const std::string& MFName, const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame);
		const std::map<std::string, std::shared_ptr<graphics::CMaterialFrame>>& GetMaterialFrameMap() const;

		void AddAnimationClipSet(const std::string& Name, const std::shared_ptr<animation::CAnimationClipSet>& AnimationClipSet);
		const std::map<std::string, std::shared_ptr<animation::CAnimationClipSet>>& GetAnimationClipSetMap() const;

		void SetSceneTextureSet(const std::shared_ptr<graphics::CTextureSet>& TextureSet);
		const std::shared_ptr<graphics::CTextureSet>& GetSceneTextureSet() const;

		void AddBGM(const std::shared_ptr<audio::CAudioClip>& AudioClip, bool autoplay, bool loop);
		const std::tuple<std::shared_ptr<audio::CAudioClip>, bool, bool>& GetSound() const;

		//
		void AddMaterialInfo(const std::shared_ptr<object::C3DObject>& Object, const std::vector<SMaterialInfo>& MaterialInfoList);
		const std::map<std::shared_ptr<object::C3DObject>, std::vector<SMaterialInfo>>& GetMaterialInfoMap() const;

		void AddTextureInfo(const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList);
		const std::map<std::shared_ptr<object::C3DObject>, std::map<std::string, std::shared_ptr<graphics::CTexture>>>& GetTextureInfoMap() const;

		void AddAnimationInfo(const std::shared_ptr<object::C3DObject>& Object, const SAnimationInfo& AnimationInfo);
		const std::map<std::shared_ptr<object::C3DObject>, SAnimationInfo>& GetAnimationInfoMap() const;
		
		//
		bool Create(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine);

		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState);
		bool LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
		bool FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
	};
}