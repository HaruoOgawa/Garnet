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

	void CTransform::CastModelMatrixToTransform(const glm::mat4& ModelMatrix, glm::vec3& Translation, glm::quat& Rotation, glm::vec3& Scale, bool UseScale)
	{
		// “n‚³‚ê‚½ModelMatrix‚©‚çPosERotateEScale‚ğ•œŒ³‚·‚é
		// https://stackoverflow.com/questions/27655885/get-position-rotation-and-scale-from-matrix-in-opengl
		// https://www.gamedev.net/forums/topic/657267-extract-position-scaling-and-rotation-from-world-matrix/
		// https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati

		// Pos
		Translation = glm::vec3(ModelMatrix[3][0], ModelMatrix[3][1], ModelMatrix[3][2]);
		
		// Scale
		if (UseScale)
		{
			Scale = glm::vec3(
				glm::sqrt(glm::length2(glm::vec3(ModelMatrix[0][0], ModelMatrix[0][1], ModelMatrix[0][2]))),
				glm::sqrt(glm::length2(glm::vec3(ModelMatrix[1][0], ModelMatrix[1][1], ModelMatrix[1][2]))),
				glm::sqrt(glm::length2(glm::vec3(ModelMatrix[2][0], ModelMatrix[2][1], ModelMatrix[2][2])))
			);
		}
		else
		{
			Scale = glm::vec3(1.0f, 1.0f, 1.0f);
		}

		// Rot
		// ‰ñ“]‚Ìæ“¾‚Í¡Œã—lq‚ğŒ©‚Â‚Â‚¢‚ë‚¢‚ë‚Æ‰ü‘P‚ª•K—v‚©‚à
		glm::mat4 RotMat = glm::mat4(
			ModelMatrix[0][0], ModelMatrix[0][1], ModelMatrix[0][2], 0.0f,
			ModelMatrix[1][0], ModelMatrix[1][1], ModelMatrix[1][2], 0.0f,
			ModelMatrix[2][0], ModelMatrix[2][1], ModelMatrix[2][2], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		RotMat[0][0] /= Scale.x;
		RotMat[1][1] /= Scale.y;
		RotMat[2][2] /= Scale.z;

		Rotation = glm::quat_cast(RotMat);
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

	void CTransform::CalcModelMatrix(glm::mat4& ModelMatrix, const glm::vec3& Translation, const glm::quat& Rotation, bool UseScale, const glm::vec3& Scale)
	{
		glm::mat4 trsMatrix = glm::translate(glm::mat4(1.0f), Translation);
		glm::mat4 rotMatrix = glm::toMat4(Rotation);
		glm::mat4 sclMatrix = glm::scale(glm::mat4(1.0f), Scale);

		if (UseScale)
		{
			ModelMatrix = trsMatrix * rotMatrix * sclMatrix;
		}
		else
		{
			ModelMatrix = trsMatrix * rotMatrix;
		}
	}
}