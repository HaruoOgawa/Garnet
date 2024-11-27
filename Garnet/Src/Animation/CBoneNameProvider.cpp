#ifdef USE_ANIMATION

#include "CBoneNameProvider.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

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

	EHumanoidBones CBoneNameProvider::GetBoneNameU16(EBonePattern Pattern, const std::wstring& SrcNodeName)
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

		const std::wstring SearchName = ToNoZeroStr(SrcNodeName.substr(SIndex));

		const auto& it = m_TableListU16.find(Pattern);
		if (it == m_TableListU16.end()) return EHumanoidBones::None;

		const auto it2 = it->second.find(SearchName);
		if (it2 == it->second.end()) return EHumanoidBones::None;

		return it2->second;
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
		InitPMXTable();
		InitVMDTable();
	}

	void CBoneNameProvider::InitPMXTable()
	{
		std::map<std::wstring, EHumanoidBones> BoneNameTable;

		// コードにUTF8以外の文字が含まれているとEmscriptenでコンパイルできないっぽいのでバイト列を実行時にwstringに変換している
		// PMXはUTF16形式の文字コード
		// VMDはShift_JIS形式の文字コード
		// 数値のバイナリはちゃんと空きにも0を入れないとおかしくなるので注意(特にShift_JIS)
		// 例えば0x32ではなく0x3200にする必要があった
		// たぶん4桁じゃないバイナリは全部0埋めで4桁にする必要があるかも
		// なぜなら別のバイト列とみなされるから(0x32だと本当は0x3200なのに0x0032になったり)

		// PMX
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x8db3, 0xff29, 0xff2b }), animation::EHumanoidBones::RightLegIK); // 右足ＩＫ
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x3064, 0x307e, 0x5148, 0xff29, 0xff2b }), animation::EHumanoidBones::RightToesIK); // 右つま先ＩＫ
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x8db3, 0xff29, 0xff2b }), animation::EHumanoidBones::LeftLegIK); // 左足ＩＫ
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x3064, 0x307e, 0x5148, 0xff29, 0xff2b }), animation::EHumanoidBones::LeftToesIK); // 左つま先ＩＫ
		
		BoneNameTable.emplace(HexToWstr({ 0x5168, 0x3066, 0x306e, 0x89aa }), animation::EHumanoidBones::AllParent); // 全ての親
		BoneNameTable.emplace(HexToWstr({ 0x30bb, 0x30f3, 0x30bf, 0x30fc }), animation::EHumanoidBones::Center); // センター
		BoneNameTable.emplace(HexToWstr({ 0x30b0, 0x30eb, 0x30fc, 0x30d6 }), animation::EHumanoidBones::Group); // グルーブ
		BoneNameTable.emplace(HexToWstr({ 0x8170 }), animation::EHumanoidBones::Hips); // 腰
		BoneNameTable.emplace(HexToWstr({ 0x4e0b, 0x534a, 0x8eab }), animation::EHumanoidBones::LowerBody); // 下半身
		BoneNameTable.emplace(HexToWstr({ 0x4e0a, 0x534a, 0x8eab }), animation::EHumanoidBones::Spine); // 上半身
		BoneNameTable.emplace(HexToWstr({ 0x4e0a, 0x534a, 0x8eab, 0x0032 }), animation::EHumanoidBones::Chest); // 上半身2
		//BoneNameTable.emplace(HexToWstr({}), animation::EHumanoidBones::UpperChest);
		BoneNameTable.emplace(HexToWstr({ 0x9996 }), animation::EHumanoidBones::Neck); // 首
		BoneNameTable.emplace(HexToWstr({ 0x982d }), animation::EHumanoidBones::Head); // 頭
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x76ee }), animation::EHumanoidBones::LeftEye); // 左目
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x76ee }), animation::EHumanoidBones::RightEye); // 右目
		//BoneNameTable.emplace(HexToWstr({}), animation::EHumanoidBones::Jaw);
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x80a9 }), animation::EHumanoidBones::LeftShoulder); // 左肩
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x8155 }), animation::EHumanoidBones::LeftUpperArm); // 左腕
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x3072, 0x3058 }), animation::EHumanoidBones::LeftLowerArm); // 左ひじ
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x624b, 0x9996 }), animation::EHumanoidBones::LeftHand); // 左手首
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x80a9 }), animation::EHumanoidBones::RightShoulder); // 右肩
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x8155 }), animation::EHumanoidBones::RightUpperArm); // 右腕
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x3072, 0x3058 }), animation::EHumanoidBones::RightLowerArm); // 右ひじ
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x624b, 0x9996 }), animation::EHumanoidBones::RightHand); // 右手首
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x8db3 }), animation::EHumanoidBones::LeftUpperLeg); // 左足
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x3072, 0x3056 }), animation::EHumanoidBones::LeftLowerLeg); // 左ひざ
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x8db3, 0x9996 }), animation::EHumanoidBones::LeftFoot); // 左足首
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x3064, 0x307e, 0x5148 }), animation::EHumanoidBones::LeftToes); // 左つま先
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x8db3 }), animation::EHumanoidBones::RightUpperLeg); // 右足
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x3072, 0x3056 }), animation::EHumanoidBones::RightLowerLeg); // 右ひざ
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x8db3, 0x9996 }), animation::EHumanoidBones::RightFoot); // 右足首
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x3064, 0x307e, 0x5148 }), animation::EHumanoidBones::RightToes); // 右つま先

		// 手の指のボーン。足のボーンは一般的にはないらしい(まぁ使う場面限られてるよね)
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x89aa, 0x6307, 0xff10 }), animation::EHumanoidBones::LeftThumbProximal); // 左親指０
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x89aa, 0x6307, 0xff11 }), animation::EHumanoidBones::LeftThumbIntermediate); // 左親指１
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x89aa, 0x6307, 0xff12 }), animation::EHumanoidBones::LeftThumbDistal); // 左親指２
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x4eba, 0x6307, 0xff11 }), animation::EHumanoidBones::LeftIndexProximal); // 左人指１
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x4eba, 0x6307, 0xff12 }), animation::EHumanoidBones::LeftIndexIntermediate); // 左人指２
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x4eba, 0x6307, 0xff13 }), animation::EHumanoidBones::LeftIndexDistal); // 左人指３
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x4e2d, 0x6307, 0xff11 }), animation::EHumanoidBones::LeftMiddleProximal); // 左中指１
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x4e2d, 0x6307, 0xff12 }), animation::EHumanoidBones::LeftMiddleIntermediate); // 左中指２
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x4e2d, 0x6307, 0xff13 }), animation::EHumanoidBones::LeftMiddleDistal); // 左中指３
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x85ac, 0x6307, 0xff11 }), animation::EHumanoidBones::LeftRingProximal); // 左薬指１
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x85ac, 0x6307, 0xff12 }), animation::EHumanoidBones::LeftRingIntermediate); // 左薬指２
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x85ac, 0x6307, 0xff13 }), animation::EHumanoidBones::LeftRingDistal); // 左薬指３
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x5c0f, 0x6307, 0xff11 }), animation::EHumanoidBones::LeftLittleProximal); // 左小指１
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x5c0f, 0x6307, 0xff12 }), animation::EHumanoidBones::LeftLittleIntermediate); // 左小指２
		BoneNameTable.emplace(HexToWstr({ 0x5de6, 0x5c0f, 0x6307, 0xff13 }), animation::EHumanoidBones::LeftLittleDistal); // 左小指３
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x89aa, 0x6307, 0x0030 }), animation::EHumanoidBones::RightThumbProximal); // 右親指0
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x89aa, 0x6307, 0xff11 }), animation::EHumanoidBones::RightThumbIntermediate); // 右親指１
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x89aa, 0x6307, 0xff12 }), animation::EHumanoidBones::RightThumbDistal); // 右親指２
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x4eba, 0x6307, 0xff11 }), animation::EHumanoidBones::RightIndexProximal); // 右人指１
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x4eba, 0x6307, 0xff12 }), animation::EHumanoidBones::RightIndexIntermediate); // 右人指２
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x4eba, 0x6307, 0xff13 }), animation::EHumanoidBones::RightIndexDistal); // 右人指３
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x4e2d, 0x6307, 0xff11 }), animation::EHumanoidBones::RightMiddleProximal); // 右中指１
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x4e2d, 0x6307, 0xff12 }), animation::EHumanoidBones::RightMiddleIntermediate); // 右中指２
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x4e2d, 0x6307, 0xff13 }), animation::EHumanoidBones::RightMiddleDistal); // 右中指３
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x85ac, 0x6307, 0xff11 }), animation::EHumanoidBones::RightRingProximal); // 右薬指１
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x85ac, 0x6307, 0xff12 }), animation::EHumanoidBones::RightRingIntermediate); // 右薬指２
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x85ac, 0x6307, 0xff13 }), animation::EHumanoidBones::RightRingDistal); // 右薬指３
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x5c0f, 0x6307, 0xff11 }), animation::EHumanoidBones::RightLittleProximal); // 右小指１
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x5c0f, 0x6307, 0xff12 }), animation::EHumanoidBones::RightLittleIntermediate); // 右小指２
		BoneNameTable.emplace(HexToWstr({ 0x53f3, 0x5c0f, 0x6307, 0xff13 }), animation::EHumanoidBones::RightLittleDistal); // 右小指３

		m_TableListU16.emplace(EBonePattern::PMX, BoneNameTable);
	}

	void CBoneNameProvider::InitVMDTable()
	{
		std::map<std::wstring, EHumanoidBones> BoneNameTable;

		// コードにUTF8以外の文字が含まれているとEmscriptenでコンパイルできないっぽいのでバイト列を実行時にwstringに変換している
		// PMXはUTF16形式の文字コード
		// VMDはShift_JIS形式の文字コード
		// 数値のバイナリはちゃんと空きにも0を入れないとおかしくなるので注意(特にShift_JIS)
		// 例えば0x32ではなく0x3200にする必要があった
		// たぶん4桁じゃないバイナリは全部0埋めで4桁にする必要があるかも
		// なぜなら別のバイト列とみなされるから(0x32だと本当は0x3200なのに0x0032になったり)

		// VMD
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x91ab, 0x8268, 0x826a }), animation::EHumanoidBones::RightLegIK); // 右足ＩＫ
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x82c2, 0x82dc, 0x90e6, 0x8268, 0x826a }), animation::EHumanoidBones::RightToesIK); // 右つま先ＩＫ
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x91ab, 0x8268, 0x826a }), animation::EHumanoidBones::LeftLegIK); // 左足ＩＫ
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x82c2, 0x82dc, 0x90e6, 0x8268, 0x826a }), animation::EHumanoidBones::LeftToesIK); // 左つま先ＩＫ

		BoneNameTable.emplace(HexToWstr({ 0x9153, 0x82c4, 0x82cc, 0x9065 }), animation::EHumanoidBones::AllParent); // 全ての親
		BoneNameTable.emplace(HexToWstr({ 0x835a, 0x8393, 0x835e, 0x815b }), animation::EHumanoidBones::Center); // センター
		BoneNameTable.emplace(HexToWstr({ 0x834f, 0x838b, 0x815b, 0x8375 }), animation::EHumanoidBones::Group); // グルーブ
		BoneNameTable.emplace(HexToWstr({ 0x8d98 }), animation::EHumanoidBones::Hips); // 腰
		BoneNameTable.emplace(HexToWstr({ 0x89ba, 0x94bc, 0x9067 }), animation::EHumanoidBones::LowerBody); // 下半身
		BoneNameTable.emplace(HexToWstr({ 0x8fe3, 0x94bc, 0x9067 }), animation::EHumanoidBones::Spine); // 上半身
		BoneNameTable.emplace(HexToWstr({ 0x8fe3, 0x94bc, 0x9067, 0x3200 }), animation::EHumanoidBones::Chest); // 上半身2
		//BoneNameTable.emplace(HexToWstr({}), animation::EHumanoidBones::UpperChest);
		BoneNameTable.emplace(HexToWstr({ 0x8ef1 }), animation::EHumanoidBones::Neck); // 首
		BoneNameTable.emplace(HexToWstr({ 0x93aa }), animation::EHumanoidBones::Head); // 頭
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x96da }), animation::EHumanoidBones::LeftEye); // 左目
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x96da }), animation::EHumanoidBones::RightEye); // 右目
		//BoneNameTable.emplace(HexToWstr({}), animation::EHumanoidBones::Jaw);
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x8ca8 }), animation::EHumanoidBones::LeftShoulder); // 左肩
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x9872 }), animation::EHumanoidBones::LeftUpperArm); // 左腕
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x82d0, 0x82b6 }), animation::EHumanoidBones::LeftLowerArm); // 左ひじ
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x8ee8, 0x8ef1 }), animation::EHumanoidBones::LeftHand); // 左手首
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x8ca8 }), animation::EHumanoidBones::RightShoulder); // 右肩
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x9872 }), animation::EHumanoidBones::RightUpperArm); // 右腕
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x82d0, 0x82b6 }), animation::EHumanoidBones::RightLowerArm); // 右ひじ
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x8ee8, 0x8ef1 }), animation::EHumanoidBones::RightHand); // 右手首
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x91ab }), animation::EHumanoidBones::LeftUpperLeg); // 左足
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x82d0, 0x82b4 }), animation::EHumanoidBones::LeftLowerLeg); // 左ひざ
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x91ab, 0x8ef1 }), animation::EHumanoidBones::LeftFoot); // 左足首
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x82c2, 0x82dc, 0x90e6 }), animation::EHumanoidBones::LeftToes); // 左つま先
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x91ab }), animation::EHumanoidBones::RightUpperLeg); // 右足
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x82d0, 0x82b4 }), animation::EHumanoidBones::RightLowerLeg); // 右ひざ
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x91ab, 0x8ef1 }), animation::EHumanoidBones::RightFoot); // 右足首
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x82c2, 0x82dc, 0x90e6 }), animation::EHumanoidBones::RightToes); // 右つま先

		// 手の指のボーン。足のボーンは一般的にはないらしい(まぁ使う場面限られてるよね)
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x9065, 0x8e77, 0x824f }), animation::EHumanoidBones::LeftThumbProximal); // 左親指０
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x9065, 0x8e77, 0x8250 }), animation::EHumanoidBones::LeftThumbIntermediate); // 左親指１
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x9065, 0x8e77, 0x8251 }), animation::EHumanoidBones::LeftThumbDistal); // 左親指２
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x906c, 0x8e77, 0x8250 }), animation::EHumanoidBones::LeftIndexProximal); // 左人指１
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x906c, 0x8e77, 0x8251 }), animation::EHumanoidBones::LeftIndexIntermediate); // 左人指２
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x906c, 0x8e77, 0x8252 }), animation::EHumanoidBones::LeftIndexDistal); // 左人指３
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x9286, 0x8e77, 0x8250 }), animation::EHumanoidBones::LeftMiddleProximal); // 左中指１
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x9286, 0x8e77, 0x8251 }), animation::EHumanoidBones::LeftMiddleIntermediate); // 左中指２
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x9286, 0x8e77, 0x8252 }), animation::EHumanoidBones::LeftMiddleDistal); // 左中指３
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x96f2, 0x8e77, 0x8250 }), animation::EHumanoidBones::LeftRingProximal); // 左薬指１
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x96f2, 0x8e77, 0x8251 }), animation::EHumanoidBones::LeftRingIntermediate); // 左薬指２
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x96f2, 0x8e77, 0x8252 }), animation::EHumanoidBones::LeftRingDistal); // 左薬指３
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x8fac, 0x8e77, 0x8250 }), animation::EHumanoidBones::LeftLittleProximal); // 左小指１
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x8fac, 0x8e77, 0x8251 }), animation::EHumanoidBones::LeftLittleIntermediate); // 左小指２
		BoneNameTable.emplace(HexToWstr({ 0x8db6, 0x8fac, 0x8e77, 0x8252 }), animation::EHumanoidBones::LeftLittleDistal); // 左小指３
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x9065, 0x8e77, 0x3000 }), animation::EHumanoidBones::RightThumbProximal); // 右親指0
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x9065, 0x8e77, 0x8250 }), animation::EHumanoidBones::RightThumbIntermediate); // 右親指１
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x9065, 0x8e77, 0x8251 }), animation::EHumanoidBones::RightThumbDistal); // 右親指２
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x906c, 0x8e77, 0x8250 }), animation::EHumanoidBones::RightIndexProximal); // 右人指１
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x906c, 0x8e77, 0x8251 }), animation::EHumanoidBones::RightIndexIntermediate); // 右人指２
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x906c, 0x8e77, 0x8252 }), animation::EHumanoidBones::RightIndexDistal); // 右人指３
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x9286, 0x8e77, 0x8250 }), animation::EHumanoidBones::RightMiddleProximal); // 右中指１
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x9286, 0x8e77, 0x8251 }), animation::EHumanoidBones::RightMiddleIntermediate); // 右中指２
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x9286, 0x8e77, 0x8252 }), animation::EHumanoidBones::RightMiddleDistal); // 右中指３
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x96f2, 0x8e77, 0x8250 }), animation::EHumanoidBones::RightRingProximal); // 右薬指１
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x96f2, 0x8e77, 0x8251 }), animation::EHumanoidBones::RightRingIntermediate); // 右薬指２
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x96f2, 0x8e77, 0x8252 }), animation::EHumanoidBones::RightRingDistal); // 右薬指３
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x8fac, 0x8e77, 0x8250 }), animation::EHumanoidBones::RightLittleProximal); // 右小指１
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x8fac, 0x8e77, 0x8251 }), animation::EHumanoidBones::RightLittleIntermediate); // 右小指２
		BoneNameTable.emplace(HexToWstr({ 0x8945, 0x8fac, 0x8e77, 0x8252 }), animation::EHumanoidBones::RightLittleDistal); // 右小指３

		m_TableListU16.emplace(EBonePattern::VMD, BoneNameTable);
	}

	// HeplerFunc ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::wstring CBoneNameProvider::HexToWstr(std::vector<int> byteArray)
	{
		std::wstring Dst = std::wstring();

		for (int byte : byteArray)
		{
			wchar_t w = *reinterpret_cast<const wchar_t*>(&byte);

			Dst.push_back(w);
		}

		return Dst;
	}

	std::wstring CBoneNameProvider::ToNoZeroStr(const std::wstring& Src)
	{
		std::wstring Dst = std::wstring();

#ifdef __EMSCRIPTEN__
		int* result_ptr = nullptr;
		int result_size = 0;

		EM_ASM({
			const ptr = $0;
			const length = $1;
			const data = new Uint16Array(Module.HEAPU16.buffer, ptr, length);
			
			const validData = [];
			//const hexArray = [];

			for (let i = 0; i < data.length; i++)
			{
				if (data[i] === 0) break;

				validData.push(data[i]);
				//hexArray.push(data[i].toString(16));
			}

			let size = validData.length;

			const resultPtr = Module._malloc(size * 4);
			for (let i = 0; i < size; i++)
			{
				HEAP32[resultPtr / 4 + i] = validData[i];
			}

			// 値をメモリに渡す
			Module.setValue($2, resultPtr, "i32");
			Module.setValue($3, size, "i32");

			return resultPtr;

		}, &Src[0], static_cast<int>(Src.size()), &result_ptr, &result_size);

		std::vector<int> ByteArray(result_ptr, result_ptr + result_size);

		free(reinterpret_cast<void*>(result_ptr));

		Dst = HexToWstr(ByteArray);
#else
		for (auto c : Src)
		{
			if (c == '\0') break;

			Dst.push_back(c);
		}
#endif

		return Dst;
	}
}

#endif // USE_ANIMATION