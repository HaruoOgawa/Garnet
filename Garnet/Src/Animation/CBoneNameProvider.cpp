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
				
				bool Result = true;
#ifdef __EMSCRIPTEN__
				for (int windex = 0; windex < BoneName.size(); windex++)
				{
					wchar_t wchr = BoneName[windex];

					int Flag = EM_ASM_INT({
						const w0 = Module.UTF16ToString($0);
						const w1 = Module.UTF16ToString($1);
						const windex = $2;

						return (w0 == w1[windex]) ? 1 : 0;
					}, &BoneName[windex], &SearchName[0], windex);

					if (Flag == 0)
					{
						Result = false;

						break;
					}
				}
#else
				int Loop = 0;

				for (auto wchr = BoneName.begin(); wchr != BoneName.end(); wchr++)
				{
					if ((*wchr) != SearchName[Loop])
					{
						Result = false;

						break;
					}

					Loop++;
				}
#endif

				// 調べたいボーン名の1つ後ろにSpace・Empty以外があったら違うボーンとして判定する
				if (SearchName.size() > BoneName.size())
				{
#ifdef __EMSCRIPTEN__
					int Flag = EM_ASM_INT({
						const w0 = Module.UTF16ToString($0);

						return (w0 != "" && w0 != " ") ? 1 : 0;
					}, &SearchName[0]);

					if (Flag == 0)
					{
						Result = false;
					}
#else
					auto wchr = SearchName[Loop];
					std::wstring Empty = std::wstring(L""), Space = std::wstring(L" ");
					Empty.resize(1);
					if (wchr != Empty[0] && wchr != Space[0])
					{
						Result = false;
					}
#endif
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
		// コードにUTF8以外の文字が含まれているとEmscriptenでコンパイルできないっぽいのでバイト列を実行時にwstringに変換している
		// また、Shift_JISからUTF16への変換が大変なので2番目にShift_JISのバイト列を追加している
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::Center, std::vector<std::wstring>{ HexToWstr({ 0x30bb, 0x30f3, 0x30bf, 0x30fc }), HexToWstr({ 0x835a, 0x8393, 0x835e, 0x815b }) }); // センター
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::Hips, std::vector<std::wstring>{ HexToWstr({ 0x8170 }), HexToWstr({ 0x8d98 }) }); // 腰
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LowerBody, std::vector<std::wstring>{ HexToWstr({ 0x4e0b, 0x534a, 0x8eab }), HexToWstr({ 0x89ba, 0x94bc, 0x9067 }) }); // 下半身
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::Spine, std::vector<std::wstring>{ HexToWstr({ 0x4e0a, 0x534a, 0x8eab }), HexToWstr({ 0x8fe3, 0x94bc, 0x9067 }) }); // 上半身
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::Chest, std::vector<std::wstring>{ HexToWstr({ 0x4e0a, 0x534a, 0x8eab, 0x0032 }), HexToWstr({ 0x8fe3, 0x94bc, 0x9067, 0x32 }) }); // 上半身2
		//m_BoneNameTableU16.emplace(animation::EHumanoidBones::UpperChest, std::vector<std::wstring>{ HexToWstr({}), HexToWstr({}) });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::Neck, std::vector<std::wstring>{ HexToWstr({ 0x9996 }), HexToWstr({ 0x8ef1 }) }); // 首
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::Head, std::vector<std::wstring>{ HexToWstr({ 0x982d }), HexToWstr({ 0x93aa }) }); // 頭
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftEye, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x76ee }), HexToWstr({ 0x8db6, 0x96da }) }); // 左目
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightEye, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x76ee }), HexToWstr({ 0x8945, 0x96da }) }); // 右目
		//m_BoneNameTableU16.emplace(animation::EHumanoidBones::Jaw, std::vector<std::wstring>{ HexToWstr({}), HexToWstr({}) });
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftShoulder, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x80a9 }), HexToWstr({ 0x8db6, 0x8ca8 }) }); // 左肩
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftUpperArm, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x8155 }), HexToWstr({ 0x8db6, 0x9872 }) }); // 左腕
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftLowerArm, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x3072, 0x3058 }), HexToWstr({ 0x8db6, 0x82d0, 0x82b6 }) }); // 左ひじ
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftHand, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x624b, 0x9996 }), HexToWstr({ 0x8db6, 0x8ee8, 0x8ef1 }) }); // 左手首
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightShoulder, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x80a9 }), HexToWstr({ 0x8945, 0x8ca8 }) }); // 右肩
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightUpperArm, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x8155 }), HexToWstr({ 0x8945, 0x9872 }) }); // 右腕
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightLowerArm, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x3072, 0x3058 }), HexToWstr({ 0x8945, 0x82d0, 0x82b6 }) }); // 右ひじ
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightHand, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x624b, 0x9996 }), HexToWstr({ 0x8945, 0x8ee8, 0x8ef1 }) }); // 右手首
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftUpperLeg, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x8db3 }), HexToWstr({ 0x8db6, 0x91ab }) }); // 左足
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftLowerLeg, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x3072, 0x3056 }), HexToWstr({ 0x8db6, 0x82d0, 0x82b4 }) }); // 左ひざ
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftFoot, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x8db3, 0x9996 }), HexToWstr({ 0x8db6, 0x91ab, 0x8ef1 }) }); // 左足首
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftToes, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x3064, 0x307e, 0x5148 }), HexToWstr({ 0x8db6, 0x82c2, 0x82dc, 0x90e6 }) }); // 左つま先
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightUpperLeg, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x8db3 }), HexToWstr({ 0x8945, 0x91ab }) }); // 右足
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightLowerLeg, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x3072, 0x3056 }), HexToWstr({ 0x8945, 0x82d0, 0x82b4 }) }); // 右ひざ
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightFoot, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x8db3, 0x9996 }), HexToWstr({ 0x8945, 0x91ab, 0x8ef1 }) }); // 右足首
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightToes, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x3064, 0x307e, 0x5148 }), HexToWstr({ 0x8945, 0x82c2, 0x82dc, 0x90e6 }) }); // 右つま先

		// 手の指のボーン。足のボーンは一般的にはないらしい(まぁ使う場面限られてるよね)
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftThumbProximal, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x89aa, 0x6307, 0xff10 }), HexToWstr({ 0x8db6, 0x9065, 0x8e77, 0x824f }) }); // 左親指０
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftThumbIntermediate, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x89aa, 0x6307, 0xff11 }), HexToWstr({ 0x8db6, 0x9065, 0x8e77, 0x8250 }) }); // 左親指１
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftThumbDistal, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x89aa, 0x6307, 0xff12 }), HexToWstr({ 0x8db6, 0x9065, 0x8e77, 0x8251 }) }); // 左親指２
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftIndexProximal, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x4eba, 0x6307, 0xff11 }), HexToWstr({ 0x8db6, 0x906c, 0x8e77, 0x8250 }) }); // 左人指１
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftIndexIntermediate, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x4eba, 0x6307, 0xff12 }), HexToWstr({ 0x8db6, 0x906c, 0x8e77, 0x8251 }) }); // 左人指２
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftIndexDistal, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x4eba, 0x6307, 0xff13 }), HexToWstr({ 0x8db6, 0x906c, 0x8e77, 0x8252 }) }); // 左人指３
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftMiddleProximal, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x4e2d, 0x6307, 0xff11 }), HexToWstr({ 0x8db6, 0x9286, 0x8e77, 0x8250 }) }); // 左中指１
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftMiddleIntermediate, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x4e2d, 0x6307, 0xff12 }), HexToWstr({ 0x8db6, 0x9286, 0x8e77, 0x8251 }) }); // 左中指２
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftMiddleDistal, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x4e2d, 0x6307, 0xff13 }), HexToWstr({ 0x8db6, 0x9286, 0x8e77, 0x8252 }) }); // 左中指３
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftRingProximal, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x85ac, 0x6307, 0xff11 }), HexToWstr({ 0x8db6, 0x96f2, 0x8e77, 0x8250 }) }); // 左薬指１
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftRingIntermediate, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x85ac, 0x6307, 0xff12 }), HexToWstr({ 0x8db6, 0x96f2, 0x8e77, 0x8251 }) }); // 左薬指２
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftRingDistal, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x85ac, 0x6307, 0xff13 }), HexToWstr({ 0x8db6, 0x96f2, 0x8e77, 0x8252 }) }); // 左薬指３
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftLittleProximal, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x5c0f, 0x6307, 0xff11 }), HexToWstr({ 0x8db6, 0x8fac, 0x8e77, 0x8250 }) }); // 左小指１
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftLittleIntermediate, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x5c0f, 0x6307, 0xff12 }), HexToWstr({ 0x8db6, 0x8fac, 0x8e77, 0x8251 }) }); // 左小指２
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftLittleDistal, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x5c0f, 0x6307, 0xff13 }), HexToWstr({ 0x8db6, 0x8fac, 0x8e77, 0x8252 }) }); // 左小指３
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightThumbProximal, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x89aa, 0x6307, 0x0030 }), HexToWstr({ 0x8945, 0x9065, 0x8e77, 0x30 }) }); // 右親指0
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightThumbIntermediate, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x89aa, 0x6307, 0xff11 }), HexToWstr({ 0x8945, 0x9065, 0x8e77, 0x8250 }) }); // 右親指１
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightThumbDistal, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x89aa, 0x6307, 0xff12 }), HexToWstr({ 0x8945, 0x9065, 0x8e77, 0x8251 }) }); // 右親指２
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightIndexProximal, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x4eba, 0x6307, 0xff11 }), HexToWstr({ 0x8945, 0x906c, 0x8e77, 0x8250 }) }); // 右人指１
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightIndexIntermediate, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x4eba, 0x6307, 0xff12 }), HexToWstr({ 0x8945, 0x906c, 0x8e77, 0x8251 }) }); // 右人指２
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightIndexDistal, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x4eba, 0x6307, 0xff13 }), HexToWstr({ 0x8945, 0x906c, 0x8e77, 0x8252 }) }); // 右人指３
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightMiddleProximal, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x4e2d, 0x6307, 0xff11 }), HexToWstr({ 0x8945, 0x9286, 0x8e77, 0x8250 }) }); // 右中指１
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightMiddleIntermediate, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x4e2d, 0x6307, 0xff12 }), HexToWstr({ 0x8945, 0x9286, 0x8e77, 0x8251 }) }); // 右中指２
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightMiddleDistal, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x4e2d, 0x6307, 0xff13 }), HexToWstr({ 0x8945, 0x9286, 0x8e77, 0x8252 }) }); // 右中指３
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightRingProximal, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x85ac, 0x6307, 0xff11 }), HexToWstr({ 0x8945, 0x96f2, 0x8e77, 0x8250 }) }); // 右薬指１
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightRingIntermediate, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x85ac, 0x6307, 0xff12 }), HexToWstr({ 0x8945, 0x96f2, 0x8e77, 0x8251 }) }); // 右薬指２
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightRingDistal, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x85ac, 0x6307, 0xff13 }), HexToWstr({ 0x8945, 0x96f2, 0x8e77, 0x8252 }) }); // 右薬指３
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightLittleProximal, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x5c0f, 0x6307, 0xff11 }), HexToWstr({ 0x8945, 0x8fac, 0x8e77, 0x8250 }) }); // 右小指１
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightLittleIntermediate, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x5c0f, 0x6307, 0xff12 }), HexToWstr({ 0x8945, 0x8fac, 0x8e77, 0x8251 }) }); // 右小指２
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightLittleDistal, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x5c0f, 0x6307, 0xff13 }), HexToWstr({ 0x8945, 0x8fac, 0x8e77, 0x8252 }) }); // 右小指３

		// IK用ボーン
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightLegIK, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x8db3, 0xff29, 0xff2b }), HexToWstr({ 0x8945, 0x91ab, 0x8268, 0x826a }) }); // 右足ＩＫ
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::RightToesIK, std::vector<std::wstring>{ HexToWstr({ 0x53f3, 0x3064, 0x307e, 0x5148, 0xff29, 0xff2b }), HexToWstr({ 0x8945, 0x82c2, 0x82dc, 0x90e6, 0x8268, 0x826a }) }); // 右つま先ＩＫ
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftLegIK, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x8db3, 0xff29, 0xff2b }), HexToWstr({ 0x8db6, 0x91ab, 0x8268, 0x826a }) }); // 左足ＩＫ
		m_BoneNameTableU16.emplace(animation::EHumanoidBones::LeftToesIK, std::vector<std::wstring>{ HexToWstr({ 0x5de6, 0x3064, 0x307e, 0x5148, 0xff29, 0xff2b }), HexToWstr({ 0x8db6, 0x82c2, 0x82dc, 0x90e6, 0x8268, 0x826a }) }); // 左つま先ＩＫ
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
}

#endif // USE_ANIMATION