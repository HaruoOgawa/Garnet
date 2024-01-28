#ifdef USE_ANIMATION

#include "CBoneNameProvider.h"

namespace animation
{
	CBoneNameProvider::CBoneNameProvider()
	{
		InitTable();
		InitTableU16();
	}

	EHumanoidBones CBoneNameProvider::GetBoneName(const std::string& SrcNodeName)
	{
		// Bone名前の余分な文字列を切り取る
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

	EHumanoidBones CBoneNameProvider::GetBoneNameU16(const std::wstring& SrcNodeName)
	{
		// Bone名前の余分な文字列を切り取る
		size_t SIndex = 0;
		if (SrcNodeName.find(L":") != -1)
		{
			SIndex = SrcNodeName.find(L":") + 1;
		}
		else if (SrcNodeName.find(L"_") != -1)
		{
			SIndex = SrcNodeName.find(L"_") + 1;
		}

		const std::wstring SearchName = SrcNodeName.substr(SIndex);

		for (const auto& Table : m_BoneNameTableU16)
		{
			const auto& BoneNameList = Table.second;

			for (const std::wstring& BoneName : BoneNameList)
			{
				// ボーン名のバイトの末尾に0がついていたりとデータの長さが不定なので、BoneNameの長さだけSearchNameを調べて一致していればオッケーということにする
				int Loop = 0;
				bool Result = true;
				for (auto wchr = BoneName.begin(); wchr != BoneName.end(); wchr++)
				{
					if ((*wchr) != SearchName[Loop])
					{
						Result = false;

						break;
					}

					Loop++;
				}

				// 調べたいボーン名の1つ後ろにSpace・Empty以外があったら違うボーンとして判定する
				if (SearchName.size() > BoneName.size())
				{
					auto wchr = SearchName[Loop];
					std::wstring Empty = std::wstring(L""), Space = std::wstring(L" ");
					Empty.resize(1);
					if (wchr != Empty[0] && wchr != Space[0])
					{
						Result = false;
					}
				}

				if (Result)
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

		// 手の指のボーン。足のボーンは一般的にはないらしい(まぁ使う場面限られてるよね)
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftThumbProximal, std::vector<std::string>{"LeftThumbProximal", "LeftHandThumb1", "leftThumbMetacarpal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftThumbIntermediate, std::vector<std::string>{"LeftThumbIntermediate", "LeftHandThumb2", "leftThumbProximal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftThumbDistal, std::vector<std::string>{"LeftThumbDistal", "LeftHandThumb3", "leftThumbDistal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftIndexProximal, std::vector<std::string>{"LeftIndexProximal", "LeftHandIndex1", "leftIndexProximal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftIndexIntermediate, std::vector<std::string>{"LeftIndexIntermediate", "LeftHandIndex2", "leftIndexIntermediate"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftIndexDistal, std::vector<std::string>{"LeftIndexDistal", "LeftHandIndex3", "leftIndexDistal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftMiddleProximal, std::vector<std::string>{"LeftMiddleProximal", "LeftHandMiddle1", "leftMiddleProximal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftMiddleIntermediate, std::vector<std::string>{"LeftMiddleIntermediate", "LeftHandMiddle2", "leftMiddleIntermediate"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftMiddleDistal, std::vector<std::string>{"LeftMiddleDistal", "LeftHandMiddle3", "leftMiddleDistal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftRingProximal, std::vector<std::string>{"LeftRingProximal", "LeftHandRing1", "leftRingProximal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftRingIntermediate, std::vector<std::string>{"LeftRingIntermediate", "LeftHandRing2", "leftRingIntermediate"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftRingDistal, std::vector<std::string>{"LeftRingDistal", "LeftHandRing3", "leftRingDistal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftLittleProximal, std::vector<std::string>{"LeftLittleProximal", "LeftHandPinky1", "leftLittleProximal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftLittleIntermediate, std::vector<std::string>{"LeftLittleIntermediate", "LeftHandPinky2", "leftLittleIntermediate"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::LeftLittleDistal, std::vector<std::string>{"LeftLittleDistal", "LeftHandPinky3", "leftLittleDistal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightThumbProximal, std::vector<std::string>{"RightThumbProximal", "RightHandThumb1", "rightThumbMetacarpal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightThumbIntermediate, std::vector<std::string>{"RightThumbIntermediate", "RightHandThumb2", "rightThumbProximal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightThumbDistal, std::vector<std::string>{"RightThumbDistal", "RightHandThumb3", "rightThumbDistal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightIndexProximal, std::vector<std::string>{"RightIndexProximal", "RightHandIndex1", "rightIndexProximal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightIndexIntermediate, std::vector<std::string>{"RightIndexIntermediate", "RightHandIndex2", "rightIndexIntermediate"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightIndexDistal, std::vector<std::string>{"RightIndexDistal", "RightHandIndex3", "rightIndexDistal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightMiddleProximal, std::vector<std::string>{"RightMiddleProximal", "RightHandMiddle1", "rightMiddleProximal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightMiddleIntermediate, std::vector<std::string>{"RightMiddleIntermediate", "RightHandMiddle2", "rightMiddleIntermediate"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightMiddleDistal, std::vector<std::string>{"RightMiddleDistal", "RightHandMiddle3", "rightMiddleDistal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightRingProximal, std::vector<std::string>{"RightRingProximal", "RightHandRing1", "rightRingProximal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightRingIntermediate, std::vector<std::string>{"RightRingIntermediate", "RightHandRing2", "rightRingIntermediate"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightRingDistal, std::vector<std::string>{"RightRingDistal", "RightHandRing3", "rightRingDistal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightLittleProximal, std::vector<std::string>{"RightLittleProximal", "RightHandPinky1", "rightLittleProximal"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightLittleIntermediate, std::vector<std::string>{"RightLittleIntermediate", "RightHandPinky2", "rightLittleIntermediate"});
		m_BoneNameTable.emplace(animation::EHumanoidBones::RightLittleDistal, std::vector<std::string>{"RightLittleDistal", "RightHandPinky3", "rightLittleDistal"});
	}

	void CBoneNameProvider::InitTableU16()
	{
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::Hips, std::vector<std::wstring>{ L"下半身" }); // もしかしたらセンターか腰かも?
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::Spine, std::vector<std::wstring>{ L"上半身" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::Chest, std::vector<std::wstring>{ L"上半身2" });
		//m_BoneNameTableU16.emplace(animation::EHumanoidBones::UpperChest, std::vector<std::wstring>{ L"" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::Neck, std::vector<std::wstring>{ L"首" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::Head, std::vector<std::wstring>{ L"頭" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftEye, std::vector<std::wstring>{ L"左目" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightEye, std::vector<std::wstring>{ L"右目" });
		//m_BoneNameTableU16.emplace(animation::EHumanoidBones::Jaw, std::vector<std::wstring>{ L"" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftShoulder, std::vector<std::wstring>{ L"左肩" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftUpperArm, std::vector<std::wstring>{ L"左腕" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftLowerArm, std::vector<std::wstring>{ L"左ひじ" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftHand, std::vector<std::wstring>{ L"左手首" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightShoulder, std::vector<std::wstring>{ L"右肩" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightUpperArm, std::vector<std::wstring>{ L"右腕" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightLowerArm, std::vector<std::wstring>{ L"右ひじ" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightHand, std::vector<std::wstring>{ L"右手首" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftUpperLeg, std::vector<std::wstring>{ L"左足" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftLowerLeg, std::vector<std::wstring>{ L"左ひざ" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftFoot, std::vector<std::wstring>{ L"左足首" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftToes, std::vector<std::wstring>{ L"左つま先" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightUpperLeg, std::vector<std::wstring>{ L"右足" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightLowerLeg, std::vector<std::wstring>{ L"右ひざ" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightFoot, std::vector<std::wstring>{ L"右足首" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightToes, std::vector<std::wstring>{ L"右つま先" });

		// 手の指のボーン。足のボーンは一般的にはないらしい(まぁ使う場面限られてるよね)
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftThumbProximal, std::vector<std::wstring>{ L"左親指０" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftThumbIntermediate, std::vector<std::wstring>{ L"左親指１" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftThumbDistal, std::vector<std::wstring>{ L"左親指２" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftIndexProximal, std::vector<std::wstring>{ L"左人指１" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftIndexIntermediate, std::vector<std::wstring>{ L"左人指２" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftIndexDistal, std::vector<std::wstring>{ L"左人指３" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftMiddleProximal, std::vector<std::wstring>{ L"左中指１" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftMiddleIntermediate, std::vector<std::wstring>{ L"左中指２" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftMiddleDistal, std::vector<std::wstring>{ L"左中指３" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftRingProximal, std::vector<std::wstring>{ L"左薬指１" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftRingIntermediate, std::vector<std::wstring>{ L"左薬指２" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftRingDistal, std::vector<std::wstring>{ L"左薬指３" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftLittleProximal, std::vector<std::wstring>{ L"左小指１" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftLittleIntermediate, std::vector<std::wstring>{ L"左小指２" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftLittleDistal, std::vector<std::wstring>{ L"左小指３" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightThumbProximal, std::vector<std::wstring>{ L"右親指0" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightThumbIntermediate, std::vector<std::wstring>{ L"右親指１" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightThumbDistal, std::vector<std::wstring>{ L"右親指２" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightIndexProximal, std::vector<std::wstring>{ L"右人指１" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightIndexIntermediate, std::vector<std::wstring>{ L"右人指２" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightIndexDistal, std::vector<std::wstring>{ L"右人指３" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightMiddleProximal, std::vector<std::wstring>{ L"右中指１" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightMiddleIntermediate, std::vector<std::wstring>{ L"右中指２" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightMiddleDistal, std::vector<std::wstring>{ L"右中指３" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightRingProximal, std::vector<std::wstring>{ L"右薬指１" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightRingIntermediate, std::vector<std::wstring>{ L"右薬指２" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightRingDistal, std::vector<std::wstring>{ L"右薬指３" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightLittleProximal, std::vector<std::wstring>{ L"右小指１" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightLittleIntermediate, std::vector<std::wstring>{ L"右小指２" });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightLittleDistal, std::vector<std::wstring>{ L"右小指３" });
	}
}

#endif // USE_ANIMATION