#include "CTransform.h"

namespace math
{
	CTransform::CTransform():
		m_Pos(0.0f),
		m_Rot(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
		m_Scale(1.0f)
	{
	}

	CTransform::CTransform(const glm::mat4& Matrix) :
		m_Pos(0.0f),
		m_Rot(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
		m_Scale(1.0f)
	{
		CTransform::CastModelMatrixToTransform(Matrix, m_Pos, m_Rot, m_Scale);
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

	void CTransform::MulRot(const glm::quat& Rot)
	{
		m_Rot *= Rot;
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
		// 渡されたModelMatrixからPos・Rotate・Scaleを復元する
		// https://stackoverflow.com/questions/27655885/get-position-rotation-and-scale-from-matrix-in-opengl
		// https://www.gamedev.net/forums/topic/657267-extract-position-scaling-and-rotation-from-world-matrix/
		// https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati

		// Pos
		CastModelMatrixToTranslation(ModelMatrix, Translation);
		
		// Scale
		CastModelMatrixToScale(ModelMatrix, Scale);

		// Rot
		CastModelMatrixToRotation(ModelMatrix, Rotation);
	}

	void CTransform::CastModelMatrixToTransform(const glm::mat4& ModelMatrix, glm::vec3& Translation, glm::quat& Rotation)
	{
		glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);

		CastModelMatrixToTransform(ModelMatrix, Translation, Rotation, Scale);
	}

	void CTransform::CastModelMatrixToTranslation(const glm::mat4& ModelMatrix, glm::vec3& Translation)
	{
		Translation = glm::vec3(ModelMatrix[3][0], ModelMatrix[3][1], ModelMatrix[3][2]);
	}

	void CTransform::CastModelMatrixToRotation(const glm::mat4& ModelMatrix, glm::quat& Rotation)
	{
		// 回転の取得は今後様子を見つついろいろと改善が必要かも
		glm::mat4 RotMat = glm::mat4(
			ModelMatrix[0][0], ModelMatrix[0][1], ModelMatrix[0][2], 0.0f,
			ModelMatrix[1][0], ModelMatrix[1][1], ModelMatrix[1][2], 0.0f,
			ModelMatrix[2][0], ModelMatrix[2][1], ModelMatrix[2][2], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		// Scaleの除算は不要かも
		/*RotMat[0][0] /= Scale.x;
		RotMat[1][1] /= Scale.y;
		RotMat[2][2] /= Scale.z;*/

		Rotation = glm::quat_cast(RotMat);
	}

	void CTransform::CastModelMatrixToScale(const glm::mat4& ModelMatrix, glm::vec3& Scale)
	{
		Scale = glm::vec3(
			glm::sqrt(glm::length2(glm::vec3(ModelMatrix[0][0], ModelMatrix[0][1], ModelMatrix[0][2]))),
			glm::sqrt(glm::length2(glm::vec3(ModelMatrix[1][0], ModelMatrix[1][1], ModelMatrix[1][2]))),
			glm::sqrt(glm::length2(glm::vec3(ModelMatrix[2][0], ModelMatrix[2][1], ModelMatrix[2][2])))
		);
	}

	glm::vec3 CTransform::GetTranslationFromModelMatrix(const glm::mat4& ModelMatrix)
	{
		glm::vec3 Translation = glm::vec3(0.0f);

		CastModelMatrixToTranslation(ModelMatrix, Translation);

		return Translation;
	}

	// 原点にある点がどこに移動するか
	void CTransform::GetMoveFromModelMatrix(const glm::mat4& ModelMatrix, glm::vec3& Move)
	{
		glm::vec4 result = ModelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		Move = glm::vec3(result.x, result.y, result.z);
	}

	void CTransform::CastLeftHandToRightHand(glm::vec3& Translation)
	{
		// TranslationをYUp右手系に変換
		// 明示的なことだが、座標についてはZの向きが反対
		Translation.z *= -1.0f;
	}

	void CTransform::CastLeftHandToRightHand(glm::quat& Rotation)
	{
		// RotationをYUp右手系に変換
		// X軸回転とY軸回転の方向が逆(Z軸回転は同じ)
		// https://www.evl.uic.edu/ralph/508S98/coordinates.html
		Rotation.x *= -1.0f;
		Rotation.y *= -1.0f;
	}

	void CTransform::CastZUpToYUp(glm::vec3& Translation)
	{
		// 単純にYとZを入れ替えて右手系に直す
		// Z軸の向きが反転することに注意
		Translation = glm::vec3(Translation.x, Translation.z, -Translation.y);
	}

	void CTransform::CastZUpToYUp(glm::quat& Rotation)
	{
		Rotation = glm::quat(0.0f, 0.0f, 1.0f, 0.0f) * Rotation * glm::quat(0.0f, 0.0f, -1.0f, 0.0f);
	}

	void CTransform::CastCentiMeter2Meter(glm::vec3& Translation)
	{
		Translation.x *= 0.01f;
		Translation.y *= 0.01f;
		Translation.z *= 0.01f;
	}

	void CTransform::FlipAroundYAxis(glm::vec3& Translation)
	{
		glm::quat Rot = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// 回転だけなのでw成分は関係ない
		glm::vec4 DstData = glm::vec4(Translation.x, Translation.y, Translation.z, 0.0f);
		DstData = glm::mat4_cast(Rot) * DstData;

		Translation = glm::vec3(DstData.x, DstData.y, DstData.z);
	}

	void CTransform::FlipAroundYAxis(glm::quat& Rotation)
	{
		Rotation *= glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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

	glm::quat CTransform::CalcTwoVectorRotate(const glm::vec3& FromVector, const glm::vec3& ToVector, float& Angle, float MaxAngle)
	{
		// https://www.opengl-tutorial.org/jp/intermediate-tutorials/tutorial-17-quaternions/
		glm::quat Result = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

		float cosTheta = glm::dot(FromVector, ToVector);

		constexpr float Epsilon = std::numeric_limits<float>::epsilon();

		if (cosTheta < -1.0f + Epsilon)
		{
			// 2つのベクトルが逆を向いている特殊ケース
			// まず平行ではない任意のベクトルを決める
			glm::vec3 SubVector = glm::vec3(0.0f);
			for (int i = 0; i < 3; i++)
			{
				glm::vec3 CheckVector = glm::vec3((i == 0) ? 1.0f : 0.0f, (i == 1) ? 1.0f : 0.0f, (i == 2) ? 1.0f : 0.0f);

				if (glm::abs(glm::dot(FromVector, CheckVector)) < 1.0f - 0.001f)
				{
					SubVector = CheckVector;

					break;
				}
			}

			// 求まったベクトルを元に回転する
			glm::vec3 RotateAxis = glm::cross(FromVector, SubVector);
			Angle = glm::acos(cosTheta);
			Angle = glm::clamp(Angle, -MaxAngle, MaxAngle);

			Result = glm::angleAxis(Angle, RotateAxis);
		}
		else
		{
			glm::vec3 RotateAxis = glm::cross(FromVector, ToVector);
			Angle = glm::acos(cosTheta);
			Angle = glm::clamp(Angle, -MaxAngle, MaxAngle);

			Result = glm::angleAxis(Angle, RotateAxis);
		}

		return Result;
	}

	void CTransform::ClampRotate(glm::quat& Rotation, const glm::vec3& LowerAngle, const glm::vec3& UpperAngle)
	{
		// 回転の角度制限をクランプで行う
		glm::vec3 SrcEuler = glm::eulerAngles(Rotation);

		glm::vec3 DstEuler = glm::vec3(
			glm::clamp(SrcEuler.x, LowerAngle.x, UpperAngle.x),
			glm::clamp(SrcEuler.y, LowerAngle.y, UpperAngle.y),
			glm::clamp(SrcEuler.z, LowerAngle.z, UpperAngle.z)
		);

		Rotation = glm::angleAxis(DstEuler.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(DstEuler.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(DstEuler.z, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void CTransform::ClampRotate(glm::mat4& ModelMatrix, const glm::vec3& LowerAngle, const glm::vec3& UpperAngle)
	{
		glm::vec3 Pos = glm::vec3(0.0f);
		glm::quat Rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

		CTransform::CastModelMatrixToTransform(ModelMatrix, Pos, Rot);

		CTransform::ClampRotate(Rot, LowerAngle, UpperAngle);

		CTransform::CalcModelMatrix(ModelMatrix, Pos, Rot, false);
	}

	void CTransform::RotateModelMatrix(glm::mat4& ModelMatrix, const glm::quat& SrcRot)
	{
		glm::vec3 Pos = glm::vec3(0.0f);
		glm::quat Rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

		CTransform::CastModelMatrixToTransform(ModelMatrix, Pos, Rot);

		CTransform::CalcModelMatrix(ModelMatrix, Pos, SrcRot, false);
	}
}