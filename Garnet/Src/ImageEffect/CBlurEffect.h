#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace imageeffect
{
	class CBlurEffect
	{
		std::vector<float> m_GaussianKernel;
	private:
		bool CalcGaussianKernel(int KernelWidth);
	public:
		CBlurEffect();
		virtual ~CBlurEffect() = default;

		bool Create(int KernelWidth);
	};
}