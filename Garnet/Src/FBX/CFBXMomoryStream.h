#pragma once

#include <vector>
#include <string>
#include <stdio.h>
#include <fbxsdk.h>
using namespace fbxsdk;

namespace fbx
{
	// http://docs.autodesk.com/FBX/2014/ENU/FBX-SDK-Documentation/index.html?url=cpp_ref/_file_stream_2main_8cxx-example.html,topicNumber=cpp_ref__file_stream_2main_8cxx_example_html8d5a461e-c3c6-4926-9292-6a9eb7a8d37e,hash=_a0
	// http://docs.autodesk.com/FBX/2014/ENU/FBX-SDK-Documentation/index.html?url=cpp_ref/class_fbx_stream.html,topicNumber=cpp_ref_class_fbx_stream_html2b5775d9-5d58-4231-a2a1-de97aada1fe6
	class CFBXMomoryStream : public FbxStream
	{
		std::vector<unsigned char> m_Data;

		int m_ReaderID;
		int m_WriteID;
	public:
		CFBXMomoryStream(FbxManager* pFbxManager, const std::vector<unsigned char>& Data);
		virtual ~CFBXMomoryStream();

		virtual FbxStream::EState GetState() override;

		virtual bool Open(void* pStreamData) override;
		virtual bool Close() override;
		virtual bool Flush() override;

		virtual size_t Write(const void* pData, FbxUInt64 Size) override;
		virtual size_t Read(void* pData, FbxUInt64 Size) const override;

		virtual int GetReaderID() const override;
		virtual int GetWriterID() const override;

		virtual void Seek(const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos) override;

		virtual FbxInt64 GetPosition() const override;
		virtual void SetPosition(FbxInt64 pPosition) override;

		virtual int GetError() const override;
		virtual void ClearError() override;

	};
}