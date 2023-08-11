#pragma once
#include <vector>
#include <string>
#include "../Interface/IResource.h"

namespace file
{
	class CFile : public resource::IResource
	{
		resource::ELoadStatus m_Status;
		const std::string m_Filename;

		std::vector<unsigned char> m_Data;

	public:
		CFile(const std::string& filename);
		virtual ~CFile();

		void Release();

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual bool Load() override;

		void SetData(const std::vector<unsigned char>& Data);
		const std::vector<unsigned char>& GetData() const;
	};
}