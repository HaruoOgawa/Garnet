#if defined(USE_FBX) && !defined(USE_SMALL_FBX)

#include "CFBXMomoryStream.h"

namespace fbx
{
	unsigned int g_DataOffset = 0;

	CFBXMomoryStream::CFBXMomoryStream(FbxManager* pFbxManager, const std::vector<unsigned char>& Data) :
		m_Data(Data),
		m_ReaderID(-1),
		m_WriteID(-1)
	{
		g_DataOffset = 0;

		const char* format = "FBX (*.fbx)";
		m_ReaderID = pFbxManager->GetIOPluginRegistry()->FindReaderIDByDescription(format);
		m_WriteID = -1;
	}

	CFBXMomoryStream::~CFBXMomoryStream()
	{
		Close();
	}

	FbxStream::EState CFBXMomoryStream::GetState()
	{
		//return (m_File) ? FbxStream::eOpen : FbxStream::eClosed;
		return FbxStream::eOpen;
	}

	bool CFBXMomoryStream::Open(void* pStreamData)
	{
		return true;
	}

	bool CFBXMomoryStream::Close()
	{
		return true;
	}

	bool CFBXMomoryStream::Flush()
	{
		return true;
	}

	size_t CFBXMomoryStream::Write(const void* pData, FbxUInt64 Size)
	{
		return 0;
	}

	size_t CFBXMomoryStream::Read(void* pData, FbxUInt64 Size) const
	{
		if (m_Data.size() == 0) return 0;

		if (g_DataOffset >= m_Data.size()) return 0;

		std::memcpy(pData, &m_Data[g_DataOffset], Size);
		
		g_DataOffset += static_cast<int>(Size);

		return 1;
	}

	int CFBXMomoryStream::GetReaderID() const
	{
		return m_ReaderID;
	}

	int CFBXMomoryStream::GetWriterID() const
	{
		return m_WriteID;
	}

	void CFBXMomoryStream::Seek(const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos)
	{
	}

	FbxInt64 CFBXMomoryStream::GetPosition() const
	{
		return 0;
	}

	void CFBXMomoryStream::SetPosition(FbxInt64 pPosition)
	{
	}

	int CFBXMomoryStream::GetError() const
	{
		return 1;
	}

	void CFBXMomoryStream::ClearError()
	{
	}
}

#endif