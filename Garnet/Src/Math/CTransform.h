#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace math
{
	class CTransform
	{
		glm::vec3 m_Pos;
		glm::quat m_Rot;
		glm::vec3 m_Scale;
	public:
		CTransform();
		virtual ~CTransform() = default;

		glm::mat4 GetModelMatrix();

		const glm::vec3& GetPos() const;
		void SetPos(const glm::vec3& Pos);

		const glm::quat& GetRot() const;
		void SetRot(const glm::quat& Rot);
		void MulRot(const glm::quat& Rot);
		void AddRotate(const glm::vec3& Axis, float Radians);

		const glm::vec3& GetScale() const;
		void SetScale(const glm::vec3& Scale);

		void CastModelMatrixToTransform(const glm::mat4& ModelMatrix);
		
		static void CastModelMatrixToTransform(const glm::mat4& ModelMatrix, glm::vec3& Translation, glm::quat& Rotation, glm::vec3& Scale);
		static void CastModelMatrixToTransform(const glm::mat4& ModelMatrix, glm::vec3& Translation, glm::quat& Rotation);
		static void CastModelMatrixToTranslation(const glm::mat4& ModelMatrix, glm::vec3& Translation);
		static void CastModelMatrixToRotation(const glm::mat4& ModelMatrix, glm::quat& Rotation);
		static void CastModelMatrixToScale(const glm::mat4& ModelMatrix, glm::vec3& Scale);

		// Œ´“_‚É‚ ‚é“_‚ª‚Ç‚±‚ÉˆÚ“®‚·‚é‚©
		static void GetMoveFromModelMatrix(const glm::mat4& ModelMatrix, glm::vec3& Move);

		static void CastLeftHandToRightHand(glm::vec3& Translation);
		static void CastLeftHandToRightHand(glm::quat& Rotation);

		static void CastZUpToYUp(glm::vec3& Translation);
		static void CastZUpToYUp(glm::quat& Rotation);

		static void CastCentiMeter2Meter(glm::vec3& Translation);

		static void FlipAroundYAxis(glm::vec3& Translation);
		static void FlipAroundYAxis(glm::quat& Rotation);

		static void CalcModelMatrix(glm::mat4& ModelMatrix, const glm::vec3& Translation, const glm::quat& Rotation, bool UseScale, const glm::vec3& Scale = glm::vec3(1.0f));

		static glm::quat CalcTwoVectorRotate(const glm::vec3& FromVector, const glm::vec3& ToVector, float MaxAngle = 2.0f * 3.1415f);

		static void ClampRotate(glm::quat& Rotation, const glm::vec3& LowerAngle, const glm::vec3& UpperAngle);
		static void ClampRotate(glm::mat4& ModelMatrix, const glm::vec3& LowerAngle, const glm::vec3& UpperAngle);

		static void RotateModelMatrix(glm::mat4& ModelMatrix, const glm::quat& SrcRot);
	};
}