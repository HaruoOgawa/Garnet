#ifdef USE_ANIMATION

#include "CBoneNameProvider.h"

namespace animation
{
	CBoneNameProvider::CBoneNameProvider()
	{
		InitTable();
	}

	EHumanoidBones CBoneNameProvider::GetBoneName(const std::string& SrcNodeName)
	{
		// BoneñºëOÇÃó]ï™Ç»ï∂éöóÒÇêÿÇËéÊÇÈ
		size_t SIndex = 0;
		if (SrcNodeName.find(":") != -1)
		{
			SIndex = SrcNodeName.find(":") + 1;
		}
		else if (SrcNodeName.find("_") != -1)
		{
			SIndex = SrcNodeName.find("_") + 1;
		}

		const std::string SearchName = SrcNodeName.substr(SIndex);

		for (const auto& Table : m_BoneNameTable)
		{
			const auto& BoneNameList = Table.second;

			for (const std::string& BoneName : BoneNameList)
			{
				if (SearchName == BoneName)
				{
					return Table.first;
				}
			}
		}

		return EHumanoidBones::None;
	}

	void CBoneNameProvider::InitTable()
	{
		m_BoneNameTable.emplace(animation::EHumanoidBones::Hips, std::vector<std::string>{"Hips", "Hips", "hips"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::Spine, std::vector<std::string>{"Spine", "Spine", "spine"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::Chest, std::vector<std::string>{"Chest", "Spine1", "chest"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::UpperChest, std::vector<std::string>{"UpperChest", "Spine2", "upperChest"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::Neck, std::vector<std::string>{"Neck", "Neck", "neck"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::Head, std::vector<std::string>{"Head", "Head", "head"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftEye, std::vector<std::string>{"LeftEye", "LeftEye", "leftEye"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightEye, std::vector<std::string>{"RightEye", "RightEye", "rightEye"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::Jaw, std::vector<std::string>{"Jaw", "Jaw", "Jaw", "jaw"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftShoulder, std::vector<std::string>{"LeftShoulder", "LeftShoulder", "leftShoulder"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftUpperArm, std::vector<std::string>{"LeftUpperArm", "LeftArm", "leftUpperArm"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftLowerArm, std::vector<std::string>{"LeftLowerArm", "LeftForeArm", "leftLowerArm"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftHand, std::vector<std::string>{"LeftHand", "LeftHand", "leftHand"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightShoulder, std::vector<std::string>{"RightShoulder", "RightShoulder", "rightShoulder"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightUpperArm, std::vector<std::string>{"RightUpperArm", "RightArm", "rightUpperArm"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightLowerArm, std::vector<std::string>{"RightLowerArm", "RightForeArm", "rightLowerArm"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightHand, std::vector<std::string>{"RightHand", "RightHand", "rightHand"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftUpperLeg, std::vector<std::string>{"LeftUpperLeg", "LeftUpLeg", "leftUpperLeg"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftLowerLeg, std::vector<std::string>{"LeftLowerLeg", "LeftLeg", "leftLowerLeg"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftFoot, std::vector<std::string>{"LeftFoot", "LeftFoot", "leftFoot"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftToes, std::vector<std::string>{"LeftToes", "LeftToeBase", "leftToes"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightUpperLeg, std::vector<std::string>{"RightUpperLeg", "RightUpLeg", "rightUpperLeg"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightLowerLeg, std::vector<std::string>{"RightLowerLeg", "RightLeg", "rightLowerLeg"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightFoot, std::vector<std::string>{"RightFoot", "RightFoot", "rightFoot"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightToes, std::vector<std::string>{"RightToes", "RightToeBase", "rightToes"});
	}
}

#endif // USE_ANIMATION