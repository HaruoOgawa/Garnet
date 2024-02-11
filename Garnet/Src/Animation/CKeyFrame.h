#pragma once
#ifdef USE_ANIMATION

#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace animation
{
	enum class EKeyFrameType
	{
		KEYFRAME_TYPE_NONE = -1,

		KEYFRAME_TYPE_SCALAR,
		KEYFRAME_TYPE_VEC2,
		KEYFRAME_TYPE_VEC3,
		KEYFRAME_TYPE_VEC4,
		KEYFRAME_TYPE_MAT2,
		KEYFRAME_TYPE_MAT3,
		KEYFRAME_TYPE_MAT4,
		KEYFRAME_TYPE_VECTOR,
		KEYFRAME_TYPE_MATRIX,
	};

	class CKeyFrame
	{
		EKeyFrameType m_Type;

		float m_Input;
		std::vector<float> m_Output; // ScalarやVEC4・MAT3などもまとめて取り扱う。要素数とタイプをみていい感じにglm::vec4とかglm::mat3に変換して返すクラスを作ってもいいかも

		std::vector<glm::vec2> m_XPointList;
		std::vector<glm::vec2> m_YPointList;
		std::vector<glm::vec2> m_ZPointList;
		std::vector<glm::vec2> m_RPointList;
	public:
		CKeyFrame(EKeyFrameType Type);
		virtual ~CKeyFrame() = default;

		EKeyFrameType GetType() const;

		void SetInput(float val);
		float GetInput() const;

		void SetOutput(const std::vector<float>& val);
		const std::vector<float>& GetOutput() const;

		void SetOutput(const float* pData, int byteSize);
		void GetOutput(float* pData);

		void SetXPointList(const std::vector<glm::vec2>& Points);
		const std::vector<glm::vec2>& GetXPointList() const;

		void SetYPointList(const std::vector<glm::vec2>& Points);
		const std::vector<glm::vec2>& GetYPointList() const;

		void SetZPointList(const std::vector<glm::vec2>& Points);
		const std::vector<glm::vec2>& GetZPointList() const;

		void SetRPointList(const std::vector<glm::vec2>& Points);
		const std::vector<glm::vec2>& GetRPointList() const;
	};
}

#endif // USE_ANIMATION