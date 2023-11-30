#pragma once
#include <vector>
#include <string>
#include "../Interface/IResource.h"

namespace resource
{
	class CFile : public resource::IResource
	{
		resource::ELoadStatus m_Status;
		const std::string m_Filename;
		std::string m_Extention;

		bool m_IsSync; // 同期的にロードするかどうか. 基本的には非同期

		std::vector<unsigned char> m_Data;

	public:
		CFile(const std::string& filename);
		virtual ~CFile();

		const std::string& GetExtention() const;

		void SetIsSync(bool IsSync);

		void Release();

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual bool Load() override;
		virtual bool Update() override;

		void SetData(const std::vector<unsigned char>& Data);
		const std::vector<unsigned char>& GetData() const;
	};
}