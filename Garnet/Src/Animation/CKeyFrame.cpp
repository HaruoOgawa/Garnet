#include "CKeyFrame.h"

namespace animation
{
	CKeyFrame::CKeyFrame(math::EValueType Type) :
		m_Type(Type),
		m_Input(0.0f)
	{
	}

	math::EValueType CKeyFrame::GetType() const
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

	void CKeyFrame::SetXPointList(const std::vector<glm::vec2>& Points)
	{
		m_XPointList = Points;
	}

	const std::vector<glm::vec2>& CKeyFrame::GetXPointList() const
	{
		return m_XPointList;
	}

	void CKeyFrame::SetYPointList(const std::vector<glm::vec2>& Points)
	{
		m_YPointList = Points;
	}

	const std::vector<glm::vec2>& CKeyFrame::GetYPointList() const
	{
		return m_YPointList;
	}

	void CKeyFrame::SetZPointList(const std::vector<glm::vec2>& Points)
	{
		m_ZPointList = Points;
	}

	const std::vector<glm::vec2>& CKeyFrame::GetZPointList() const
	{
		return m_ZPointList;
	}

	void CKeyFrame::SetRPointList(const std::vector<glm::vec2>& Points)
	{
		m_RPointList = Points;
	}

	const std::vector<glm::vec2>& CKeyFrame::GetRPointList() const
	{
		return m_RPointList;
	}
}