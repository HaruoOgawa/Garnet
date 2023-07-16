#include "CBlurEffect.h"
#include "../Debug/Message/Console.h"

namespace imageeffect
{
	CBlurEffect::CBlurEffect()
	{

	}

	bool CBlurEffect::Create(int KernelWidth)
	{
		if (!CalcGaussianKernel(KernelWidth)) return false;

		return true;
	}

	bool CBlurEffect::CalcGaussianKernel(int KernelWidth)
	{
		m_GaussianKernel.clear();

		float Sum = 0.0f;
		float w = static_cast<float>(KernelWidth);
		float sigma = 1.0f;
		float pi = 3.1415f;

		// ガウスカーネルを計算
		// https://stackoverflow.com/questions/74230190/how-to-calculate-normal-distribution-kernal-for-1d-gaussian-filter
		// https://github.com/lchop/Gaussian_filter_1D_cpp/blob/master/gaussian_filter_1D.cpp#L22
		for (float x = -w; x <= w; x++)
		{
			float v = glm::exp(-(x * x) / (2.0f * sigma * sigma)) / (sigma * glm::sqrt(2.0f * pi));
			Sum += v;

			m_GaussianKernel.push_back(v);
		}

		// 正規化
		for (auto& v : m_GaussianKernel)
		{
			v /= Sum;
		}
		
		for (int i = 0; i < m_GaussianKernel.size(); i++){ Console::Log("[%d] %f\n", i, m_GaussianKernel[i]); }

		return true;
	}
}