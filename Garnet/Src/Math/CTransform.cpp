#include "CTransform.h"

namespace math
{
	CTransform::CTransform():
		m_Pos(0.0f),
		m_Rot(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
		m_Scale(1.0f)
	{
	}

	glm::mat4 CTransform::GetModelMatrix()
	{
		glm::mat4 trsMatrix = glm::translate(glm::mat4(1.0f), m_Pos);
		glm::mat4 rotMatrix = glm::toMat4(m_Rot);
		glm::mat4 sclMatrix = glm::scale(glm::mat4(1.0f), m_Scale);

		glm::mat4 result = trsMatrix * rotMatrix * sclMatrix;

		return result;
	}

	const glm::vec3& CTransform::GetPos() const
	{
		return m_Pos;
	}

	void CTransform::SetPos(const glm::vec3& Pos)
	{
		m_Pos = Pos;
	}

	const glm::quat& CTransform::GetRot() const
	{
		return m_Rot;
	}

	void CTransform::SetRot(const glm::quat& Rot)
	{
		m_Rot = Rot;
	}

	void CTransform::AddRotate(const glm::vec3& Axis, float Radians)
	{
		m_Rot *= glm::angleAxis(glm::degrees(Radians), Axis);
	}

	const glm::vec3& CTransform::GetScale() const
	{
		return m_Scale;
	}

	void CTransform::SetScale(const glm::vec3& Scale)
	{
		m_Scale = Scale;
	}

	void CTransform::CastModelMatrixToTransform(const glm::mat4& ModelMatrix)
	{
		math::CTransform::CastModelMatrixToTransform(ModelMatrix, m_Pos, m_Rot, m_Scale);
	}

	void CTransform::CastModelMatrixToTransform(const glm::mat4& ModelMatrix, glm::vec3& Translation, glm::quat& Rotation, glm::vec3& Scale)
	{
		// “n‚³‚ê‚½ModelMatrix‚©‚çPosERotateEScale‚ğ•œŒ³‚·‚é
		// https://stackoverflow.com/questions/27655885/get-position-rotation-and-scale-from-matrix-in-opengl

		// Pos
		Translation = glm::vec3(ModelMatrix[3][0], ModelMatrix[3][1], ModelMatrix[3][2]);
		
		// Rot
		glm::mat3 RotScaleMat = glm::mat3(
			ModelMatrix[0][0], ModelMatrix[0][1], ModelMatrix[0][2],
			ModelMatrix[1][0], ModelMatrix[1][1], ModelMatrix[1][2],
			ModelMatrix[2][0], ModelMatrix[2][1], ModelMatrix[2][2]
		);

		// 1‚æ‚è‚à‘å‚«‚¢Scale‚ª‘¶İ‚·‚é
		float ScalingFactor = glm::sqrt(RotScaleMat[0][0] * RotScaleMat[0][0] + RotScaleMat[1][1] * RotScaleMat[1][1] + RotScaleMat[2][2] * RotScaleMat[2][2]);

		glm::mat3 RotMat = (1.0f / ScalingFactor) * RotScaleMat;

		Rotation = glm::quat_cast(glm::mat4(
			RotMat[0][0], RotMat[0][1], RotMat[0][2], 0.0f,
			RotMat[1][0], RotMat[1][1], RotMat[1][2], 0.0f,
			RotMat[2][0], RotMat[2][1], RotMat[2][2], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));
		
		// Scale
		//glm::mat3 ScaleMat = glm::inverse(RotMat) * RotScaleMat;
		//Scale = glm::vec3(ScaleMat[0][0], ScaleMat[1][1], ScaleMat[2][2]);

		// ‚È‚º‚©1.7‚ğ•Ô‚·‚Ì‚Å‚Ğ‚Æ‚Ü‚¸‚µ‚Î‚ç‚­‚Í1.0‚ğ‹­§“I‚É•Ô‚·
		Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	void CTransform::ToYUpRightHandedCoordinate(glm::vec3& Translation)
	{
		// Translation‚ğYUp‰EèŒn‚É•ÏŠ·
		// –¾¦“I‚È‚±‚Æ‚¾‚ªAÀ•W‚É‚Â‚¢‚Ä‚ÍZ‚ÌŒü‚«‚ª”½‘Î
		Translation.z *= -1.0f;
	}

	void CTransform::ToYUpRightHandedCoordinate(glm::quat& Rotation)
	{
		// Rotation‚ğYUp‰EèŒn‚É•ÏŠ·
		// X²‰ñ“]‚ÆY²‰ñ“]‚Ì•ûŒü‚ª‹t(Z²‰ñ“]‚Í“¯‚¶)
		// https://www.evl.uic.edu/ralph/508S98/coordinates.html
		Rotation.x *= -1.0f;
		Rotation.y *= -1.0f;
	}

	void CTransform::CastCentiMeter2Meter(glm::vec3& Val)
	{
		Val.x *= 0.01f;
		Val.y *= 0.01f;
		Val.z *= 0.01f;
	}

	void CTransform::CalcModelMatrix(glm::mat4& ModelMatrix, const glm::vec3& Translation, const glm::quat& Rotation, const glm::vec3& Scale)
	{
		glm::mat4 trsMatrix = glm::translate(glm::mat4(1.0f), Translation);
		glm::mat4 rotMatrix = glm::toMat4(Rotation);
		glm::mat4 sclMatrix = glm::scale(glm::mat4(1.0f), Scale);

		ModelMatrix = trsMatrix * rotMatrix * sclMatrix;
	}
}