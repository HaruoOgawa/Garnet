#pragma once
#if defined(USE_FBX) && defined(USE_SMALL_FBX)

// EmscriptenBuildで使用したいのでWebブラウザとデスクトップのWebGPUではSmallFBXを使用する
// その他GraphicsAPIでは通常通りAutoDeskのFBX SDKを引き続き使用する
// https://github.com/i-saint/SmallFBX

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
#include <chrono>

#include <sstream>
#include <iostream>

#ifdef __cpp_lib_span
#include <span>
#endif

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include "../Interface/IGraphicsAPI.h"
#include "../LoadWorker/CFile.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <SmallFBX.h>

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

namespace fbx
{
	class CSmallFBXImporter
	{
	private:
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, bool IsUseObject, object::C3DObject* Object,
			std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame);
	public:
		static bool ImportFBX(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, object::C3DObject* Object,
			const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame);
		static bool ImportFBXAnimation(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList);
	};
}
#endif