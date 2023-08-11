#pragma once
#include <vector>
#include <memory>
#include "../Interface/IResource.h"

namespace resource
{
	class CLoadWorker
	{
		std::vector<std::shared_ptr<resource::IResource>> m_LoadResourceList;
	public:
		CLoadWorker();
		virtual ~CLoadWorker();

		bool Update();
		bool Draw();

		bool IsLoaded();

		void AddLoadResource(const std::shared_ptr<resource::IResource>& Resource);
	};
}