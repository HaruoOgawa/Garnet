#pragma once

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
	public:
		CKeyFrame(EKeyFrameType Type);
		virtual ~CKeyFrame() = default;

		EKeyFrameType GetType() const;

		void SetInput(float val);
		float GetInput() const;

		void SetOutput(const std::vector<float>& val);
		const std::vector<float>& GetOutput() const;
	};
}