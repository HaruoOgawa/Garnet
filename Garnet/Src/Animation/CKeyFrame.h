#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../Math/CMath.h"

namespace animation
{
	class CKeyFrame
	{
		math::EValueType m_Type;

		float m_Input;
		std::vector<float> m_Output; // ScalarやVEC4・MAT3などもまとめて取り扱う。要素数とタイプをみていい感じにglm::vec4とかglm::mat3に変換して返すクラスを作ってもいいかも

		std::vector<glm::vec2> m_XPointList;
		std::vector<glm::vec2> m_YPointList;
		std::vector<glm::vec2> m_ZPointList;
		std::vector<glm::vec2> m_RPointList;
	public:
		CKeyFrame(math::EValueType Type);
		virtual ~CKeyFrame() = default;

		math::EValueType GetType() const;

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