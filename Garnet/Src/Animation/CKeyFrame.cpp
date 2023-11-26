#include "CKeyFrame.h"

namespace animation
{
	CKeyFrame::CKeyFrame(EKeyFrameType Type) :
		m_Type(Type),
		m_Input(0.0f)
	{
	}

	EKeyFrameType CKeyFrame::GetType() const
	{
		return m_Type;
	}

	void CKeyFrame::SetInput(float val)
	{
		m_Input = val;
	}

	float CKeyFrame::GetInput() const
	{
		return m_Input;
	}

	void CKeyFrame::SetOutput(const std::vector<float>& val)
	{
		m_Output = val;
	}

	const std::vector<float>& CKeyFrame::GetOutput() const
	{
		return m_Output;
	}

	void CKeyFrame::SetOutput(const float* pData, int byteSize)
	{
		int DataCount = byteSize / sizeof(float);

		m_Output.resize(DataCount);
		std::memcpy(&m_Output[0], pData, byteSize);
	}

	void CKeyFrame::GetOutput(float* pData)
	{
		std::memcpy(pData, &m_Output[0], sizeof(float) * m_Output.size());
	}
}