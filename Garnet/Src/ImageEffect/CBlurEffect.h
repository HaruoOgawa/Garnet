#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace imageeffect
{
	class CBlurEffect
	{
		std::vector<float> m_GaussianKernel;
	private:
		bool CalcGaussianKernel();
	public:
		CBlurEffect();
		virtual ~CBlurEffect() = default;

		bool Create();
	};
}