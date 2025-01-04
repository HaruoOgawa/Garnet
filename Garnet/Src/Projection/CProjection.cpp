#include "CProjection.h"

namespace projection
{
	CProjection::CProjection():
		m_FOV(45.0f),
		m_ScreenResolution(glm::vec2(1.0f)),
		m_Near(0.1f),
		m_Far(1000.0f),
		m_UseObliqueMat(false),
		m_ClipPlane(glm::vec4(0.0f))
	{
	}

	glm::mat4 CProjection::GetPerspectiveMatrix()
	{
		float Aspect = m_ScreenResolution.x / m_ScreenResolution.y;

		glm::mat4 pmat = glm::perspective(glm::radians(m_FOV), Aspect, m_Near, m_Far);
#ifndef USE_OPENGL
		pmat[1][1] *= -1.0f; /// Y座標の向きを反転。Vulkan(WebGPU)とOpenGLは逆なのかな？
#endif // !USE_OPENGL

		return pmat;
	}

	glm::mat4 CProjection::GetPrejectionMatrix()
	{
		glm::mat4 pmat = GetPerspectiveMatrix();

		if (m_UseObliqueMat)
		{
			pmat = CalculateObliqueMatrix(pmat, m_ClipPlane);
		}

		return pmat;
	}

	void CProjection::SetFOV(float FOV)
	{
		m_FOV = FOV;
	}

	float CProjection::GetFOV() const
	{
		return m_FOV;
	}

	void CProjection::SetScreenResolution(int Width, int Height)
	{
		m_ScreenResolution = glm::vec2(static_cast<float>(Width), static_cast<float>(Height));
	}

	const glm::vec2& CProjection::GetScreenResolution() const
	{
		return m_ScreenResolution;
	}

	void CProjection::SetNear(float Near)
	{
		m_Near = Near;
	}

	float CProjection::GetNear() const
	{
		return m_Near;
	}

	void CProjection::SetFar(float Far)
	{
		m_Far = Far;
	}

	float CProjection::GetFar() const
	{
		return m_Far;
	}

	void CProjection::EnabledObliqueMat(bool Flag, const glm::vec4& ClipPlane)
	{
		m_UseObliqueMat = Flag;
		m_ClipPlane = ClipPlane;
	}

	glm::mat4 CProjection::CalculateObliqueMatrix(const glm::mat4& pmat, const glm::vec4& clipPlane)
	{
		// Projection行列をカメラと指定した平面の間を描画対象から外すものに変換する
		// clipPlaneの(x, y, z, w)は平面方程式の(a, b, c, d)に該当する
		// --- 平面方程式の定義 ---------------------
		// ax + by + cz + d = 0
		// d = - n・p
		// 
		// (a, b, c)は平面の法線を表す : n = (a, b, c)
		// pは平面上の任意の点
		glm::mat4 result = pmat;

		// ToDo: あまり理屈を理解していないのでプロジェクション行列の再確認の時にここも一緒に深く学ぶ
		glm::vec4 q;
		q.x = (clipPlane.x > 0.0f) ? 1.0f : -1.0f;
		q.y = (clipPlane.y > 0.0f) ? 1.0f : -1.0f;
		q.z = 1.0f;
		q.w = 1.0f;

		//
		glm::vec4 c = clipPlane * (2.0f / glm::dot(clipPlane, q));

		//
		result[0][2] = c.x;
		result[1][2] = c.y;
		result[2][2] = c.z;
		result[3][2] = c.w;

		return result;
	}
}