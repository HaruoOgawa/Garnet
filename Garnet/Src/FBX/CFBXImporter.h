#pragma once

#ifdef USE_FBX

#include <vector>
#include <memory>
#include <string>
#include <map>

#include "../Interface/IGraphicsAPI.h"
#include "../File/CFile.h"

#include <fbxsdk.h>

namespace graphics
{
	class CTexture;
	class CTextureSet;
	class CMaterial;
	class CMesh;
	class CMaterialCreateInfo;
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
	class CFBXImporter
	{
	public:
		static bool ImportFBXAnimation();
	};
}
#endif // USE_FBX