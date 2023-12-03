#pragma once
#ifdef USE_GLTF

#include <vector>
#include <memory>
#include <string>
#include <map>

#include "../Interface/IGraphicsAPI.h"
#include "../LoadWorker/CFile.h"

namespace graphics
{
	class CTexture;
	class CTextureSet;
	class CMaterial;
	class CMesh;
	class CMaterialCreateInfo;
	class CMaterialFrame;
}

namespace renderer
{
	enum class EDataType;
}

namespace animation
{
	class CSkin;
	class CJoint;
	class CAnimationClip;
	class CAnimationSampler;
	enum class EKeyFrameType;
}

namespace object { 
	class C3DObject; 
	class CNode;
}

namespace tinygltf { 
	class Model; 
	struct Accessor;
	struct AnimationSampler;
}

namespace gltf
{
	class CGLTFImporter
	{
	private:
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, tinygltf::Model model, object::C3DObject* Object,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame);

		static bool CreateTexture(api::IGraphicsAPI* pGraphicsAPI, const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList);

		static bool CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, 
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, const std::shared_ptr<graphics::CTextureSet>& TextureSet);

		static bool CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList);

		static bool CreateMesh(const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList);

		static bool CreateNode(const tinygltf::Model& model, std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, 
			const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::vector<int>>& RootNodeIndexList);

		static bool CreateAnimationSkin(const tinygltf::Model& model, std::vector<std::shared_ptr<animation::CSkin>>& AnimationSkinList, const std::vector<std::shared_ptr<object::CNode>>& NodeList);
		static void ApplyParentJointList(const std::shared_ptr<animation::CSkin>& Skin, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		static bool CreateAnimation(const tinygltf::Model& model, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, const std::vector<std::shared_ptr<object::CNode>>& NodeList);
		static bool CreateAnimationSampler(const tinygltf::Model& model, const tinygltf::AnimationSampler& glTFSampler, std::shared_ptr<animation::CAnimationSampler>& AnimationSampler);

		// Helper Function
		static bool CalculateBufferFromAccessor(const tinygltf::Model& model, const tinygltf::Accessor& Accessor, std::vector<unsigned char>& BufferData);
		static bool RecalculateTangent(std::vector<float>& TangentDat, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned short>& Indices);
		static bool RecalculateTangentWithUINT(std::vector<float>& TangentData, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned int>& Indices);
		static animation::EKeyFrameType ConvertToEKeyFrameType(int Type);
		static int CalcStrideFromAccessor(const tinygltf::Model& model, const tinygltf::Accessor& Accessor);
		static renderer::EDataType GetComponentTypeFromAccessor(const tinygltf::Accessor& Accessor);
		static int GetByteStride(const tinygltf::Model& model, const tinygltf::Accessor& Accessor);
	public:
		static bool ImportFromMemory(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, object::C3DObject* Object,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame);
		
		static bool ImportFromString(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, const std::string& BaseDir, object::C3DObject* Object,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame);
	};
}
#endif // USE_GLTF