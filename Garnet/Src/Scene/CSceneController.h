#pragma once

#include <memory>
#include <vector>
#include <map>
#include <tuple>
#include <string>

#include "../../../Interface/IGraphicsAPI.h"
#include "../../../Interface/IPhysicsEngine.h"
#include "../../Graphics/ECullMode.h"

namespace object { class C3DObject; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace resource { class CLoadWorker; }
namespace input { class CInputState; }
namespace graphics { 
	class CDrawInfo; 
	class CMaterialFrame;
	class CTexture;
}

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
	};

	struct SLoadTextureInfo
	{
		std::string TextureName = "";
		std::shared_ptr<graphics::CTexture> Texture = nullptr;
	};

	class CSceneController
	{
		std::vector<std::shared_ptr<object::C3DObject>> m_ObjectList;

		std::map<std::string, std::shared_ptr<graphics::CMaterialFrame>> m_MaterialFrameMap;

		std::map<std::shared_ptr<object::C3DObject>, std::vector<SMaterialInfo>> m_MaterialInfoMap;

		std::map<std::shared_ptr<object::C3DObject>, std::vector<SLoadTextureInfo>> m_TextureInfoMap;
	private:
		bool PrepareTextureList(const std::shared_ptr<object::C3DObject>& Object, std::map<std::string, int>& TexIndexMap);
		bool PrepareMaterialList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, int>& TexIndexMap);
		bool CreateMaterialList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, int>& TexIndexMap);
		bool UpdateMaterialUniform(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, int>& TexIndexMap);
	public:
		CSceneController();
		virtual ~CSceneController();

		void AddObject(const std::shared_ptr<object::C3DObject>& Object);
		std::vector<std::shared_ptr<object::C3DObject>> GetObjectList() const;

		void AddMaterialFrame(const std::string& MFName, const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame);
		const std::map<std::string, std::shared_ptr<graphics::CMaterialFrame>>& GetMaterialFrameMap() const;

		void AddMaterialInfo(const std::shared_ptr<object::C3DObject>& Object, const std::vector<SMaterialInfo>& MaterialInfoList);

		void AddTextureInfo(const std::shared_ptr<object::C3DObject>& Object, const std::vector<SLoadTextureInfo>& TextureInfoList);

		bool Create(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine);

		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState);
		bool LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
		bool FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
	};
}