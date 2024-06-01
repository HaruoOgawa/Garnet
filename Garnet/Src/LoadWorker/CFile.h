#pragma once
#include <vector>
#include <string>
#include "../Interface/IResource.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/fetch.h>
#else
#include <fstream>
#include <iostream>
#endif

namespace resource
{
	class CLoadWorker;

	class CFile : public resource::IResource
	{
#ifndef __EMSCRIPTEN__
		std::ifstream m_Stream;
		int m_ByteOffset;
#endif // !__EMSCRIPTEN__

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

		virtual const std::string& GetFilename() const override;
		virtual int GetLoadPriority() const override;

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual bool Load() override;
		virtual bool LoadImmediate() override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker) override;

		virtual void Reset() override;

		virtual bool Reload(resource::CLoadWorker* pLoadWorker) override;

		virtual void AddReference(const std::shared_ptr<IResource>& Resource) override;

		void SetData(const std::vector<unsigned char>& Data);
		const std::vector<unsigned char>& GetData() const;

		static std::string GetParentDir(const std::string& Path);
		static std::string AddPunct(const std::string& Path);

		static std::wstring CastU8ToU16Str(const std::string& U8Str);
		static std::string CastU16ToU8Str(const std::wstring& U16Str);
	};
}