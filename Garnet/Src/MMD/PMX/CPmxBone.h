#pragma once

#ifdef USE_MMD
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../../Animation/SIKParam.h"

namespace mmd
{
	class CPmxBone
	{
		std::pair<std::string, std::wstring> m_BoneName;
		std::pair<std::string, std::wstring> m_BoneName_EN;

		glm::vec3 m_Pos;

		int m_ParentBoneIndex;

		int m_DeformLayer;

		// 接続(影響を受ける)ボーン
		int m_ConnectBoneIndex;

		// 回転付与・移動付与
		// 付与とは他のボーンに付いて行くということ
		// 付与親ボーンのボーンIndex
		int m_GrantParentBoneIndex;

		// 付与率
		float m_GrantRate;

		// 回転付与
		bool m_RotateGrant;

		// 移動付与
		bool m_MoveGrant;

		// ローカル軸
		bool m_UseLoacalAxis;
		glm::quat m_LocalAxis;

		// IK
		std::shared_ptr<animation::SIKParam> m_IKParam;
	private:
		void ReadBoneFlag(int BoneFlag);
	public:
		CPmxBone(const std::pair<std::string, std::wstring> & BoneName, const std::pair<std::string, std::wstring> & BoneName_EN, const glm::vec3& Pos, int ParentBoneIndex, int DeformLayer, unsigned short BoneFlag);
		virtual ~CPmxBone() = default;

		const std::pair<std::string, std::wstring>& GetBoneName() const;
		const std::pair<std::string, std::wstring>& GetBoneName_EN() const;

		const glm::vec3& GetPos() const;

		int GetParentBoneIndex() const;

		int GetDeformLayer() const;

		// 付与親ボーンのボーンIndex
		int GetGrantParentBoneIndex() const;

		// 付与率
		float GetGrantRate() const;

		// 回転付与
		void SetRotateGrant(int GrantParentBoneIndex, float GrantRate);
		bool IsRotateGrant() const;

		// 移動付与
		void SetMoveGrant(int GrantParentBoneIndex, float GrantRate);
		bool IsMoveGrant() const;

		// ローカル軸
		bool IsUseLoacalAxis() const;
		void SetLocalAxis(const glm::vec3& XAxisVector, const glm::vec3& ZAxisVector);
		const glm::quat& GetLocalAxis() const;

		// IK
		const std::shared_ptr<animation::SIKParam>& GetIKParam() const;
		void SetIKParam(const std::shared_ptr<animation::SIKParam>& Param);

		glm::mat4 GetWorldMatrix();
	};
}
#endif