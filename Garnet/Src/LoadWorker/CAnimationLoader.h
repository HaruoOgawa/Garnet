#pragma once
#ifdef USE_ANIMATION
#include <vector>
#include <string>
#include <memory>

#include "CResource.h"

#include "CFile.h"
#include "../Animation/CAnimationClipSet.h"

namespace resource
{
	class CAnimationLoader : public resource::CResource
	{
		std::shared_ptr<animation::CAnimationClipSet> m_Target;
	public:
		CAnimationLoader(const std::string& FileName, const std::shared_ptr<animation::CAnimationClipSet>& Target);
		virtual ~CAnimationLoader();

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager) override;
	};
}
#endif