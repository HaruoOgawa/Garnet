#include "CBlurEffect.h"
#include "../Debug/Message/Console.h"

namespace imageeffect
{
	CBlurEffect::CBlurEffect()
	{

	}

	bool CBlurEffect::Create()
	{
		if (!CalcGaussianKernel()) return false;

		return true;
	}

	bool CBlurEffect::CalcGaussianKernel()
	{
		m_GaussianKernel.clear();

		float Sum = 0.0f;
		float sigma = 3.0f;
		float pi = 3.1415f;
		float support = 0.995;

		// radiusはこのように求めると結構適切な値が得られる
		// http://demofox.org/gauss.html
		float radius = glm::ceil(glm::sqrt(-2.0f * sigma * sigma * glm::log(1.0f - support)));

		// ガウスカーネルを計算
		// https://stackoverflow.com/questions/74230190/how-to-calculate-normal-distribution-kernal-for-1d-gaussian-filter
		// https://github.com/lchop/Gaussian_filter_1D_cpp/blob/master/gaussian_filter_1D.cpp#L22
		for (float x = -radius; x <= radius; x++)
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
		
		/*Console::Log("[CalcGaussianKernel] radius: %f ___________________________________\n", radius);
		for (int i = 0; i < m_GaussianKernel.size(); i++){ Console::Log("[%d] %f\n", i, m_GaussianKernel[i]); }
		Console::Log("___________________________________________________________________\n");*/

		return true;
	}
}