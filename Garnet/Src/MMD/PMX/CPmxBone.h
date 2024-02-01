#pragma once

#ifdef USE_MMD
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

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

		// ê⁄ë±(âeãøÇéÛÇØÇÈ)É{Å[Éì
		int m_ConnectBoneIndex;

		//
		bool m_UseLoacalAxis;
		glm::quat m_LocalAxis;
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

		//
		bool IsUseLoacalAxis() const;
		void SetLocalAxis(const glm::vec3& XAxisVector, const glm::vec3& ZAxisVector);
		const glm::quat& GetLocalAxis() const;

		glm::mat4 GetWorldMatrix();
	};
}
#endif