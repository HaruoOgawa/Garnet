#include "CKeyFrame.h"

namespace animation
{
	CKeyFrame::CKeyFrame(EKeyFrameType Type) :
		m_Type(Type),
		m_Input(0.0f)
	{
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
}