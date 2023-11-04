#ifdef USE_FBX
#include "CFBXImporter.h"

#include "../Object/C3DObject.h"

#include "../Debug/Message/Console.h"
#include "../Math/CMath.h"

#include "../Animation/CAnimationClip.h"
#include "../Animation/CSkin.h"
#include "../Animation/CJoint.h"

namespace fbx
{
	bool CFBXImporter::ImportFBXAnimation()
	{
		FbxManager* Manager = FbxManager::Create();

		return true;
	}
}
#endif // USE_FBX