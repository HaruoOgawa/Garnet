#include "CFBXStream.h"

namespace fbx
{
	CFBXStream::CFBXStream(FbxManager* pFbxManager, const char* mode):
		m_File(NULL),
		m_Mode(NULL),
		m_FileName(NULL),
		m_ReaderID(-1),
		m_WriteID(-1)
	{
		if (mode[0] == 'r')
		{
			//m_ReaderID = 
		}
		else
		{

		}
	}

	CFBXStream::~CFBXStream()
	{
		Close();
	}

	FbxStream::EState CFBXStream::GetState()
	{
		return (m_File) ? FbxStream::eOpen : FbxStream::eClosed;
	}

	bool CFBXStream::Open(void* pStreamData)
	{
		if (m_File == NULL)
		{
			FBXSDK_fopen(m_File, m_FileName, m_Mode);
		}
		else
		{
			fseek(m_File, 0L, SEEK_SET);
		}

		return (m_File != NULL);
	}

	bool CFBXStream::Close()
	{
		if (m_File)
		{
			fclose(m_File);
		}

		m_File = NULL;

		return true;
	}

	bool CFBXStream::Flush()
	{
		return true;
	}

	size_t CFBXStream::Write(const void* pData, FbxUInt64 Size)
	{
		if (m_File == NULL) return 0;

		return (int)fwrite(pData, 1, Size, m_File);
	}

	size_t CFBXStream::Read(void* pData, FbxUInt64 Size) const
	{
		if (m_File == NULL) return 0;

		return (int)fread(pData, 1, Size, m_File);
	}

	int CFBXStream::GetReaderID() const
	{
		return m_ReaderID;
	}

	int CFBXStream::GetWriterID() const
	{
		return m_WriteID;
	}

	void CFBXStream::Seek(const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos)
	{
		switch (pSeekPos)
		{
		case FbxFile::eBegin:
			fseek(m_File, (long)pOffset, SEEK_SET);
			break;
		case FbxFile::eCurrent:
			fseek(m_File, (long)pOffset, SEEK_CUR);
			break;
		case FbxFile::eEnd:
			fseek(m_File, (long)pOffset, SEEK_END);
			break;
		default:
			break;
		}
	}

	FbxInt64 CFBXStream::GetPosition() const
	{
		if (m_File == NULL) return 0;

		return ftell(m_File);
	}

	void CFBXStream::SetPosition(FbxInt64 pPosition)
	{
		if (m_File)
		{
			fseek(m_File, static_cast<long>(pPosition), SEEK_SET);
		}
	}

	int CFBXStream::GetError() const
	{
		if (m_File == NULL) return 0;

		return ferror(m_File);
	}

	void CFBXStream::ClearError()
	{
		if (m_File != NULL)
		{
			clearerr(m_File);
		}
	}
}