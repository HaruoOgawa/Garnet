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

		// BoneFlag

		// 接続(影響を受ける)ボーン
		int m_ConnectBoneIndex;

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