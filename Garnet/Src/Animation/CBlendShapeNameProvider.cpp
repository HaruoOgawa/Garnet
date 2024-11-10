#ifdef USE_ANIMATION
#include "CBlendShapeNameProvider.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace animation
{
	CBlendShapeNameProvider::CBlendShapeNameProvider()
	{
		InitTable();
		InitTableU16();
	}

	EBlendShapeName CBlendShapeNameProvider::GetBlendShapeName(const std::string& SrcNodeName)
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

		for (const auto& Table : m_BlendShapeNameTable)
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

		return EBlendShapeName::None;
	}

	EBlendShapeName CBlendShapeNameProvider::GetBlendShapeNameU16(EBonePattern Pattern, const std::wstring& SrcNodeName)
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
		if (it == m_TableListU16.end()) EBlendShapeName::None;

		const auto it2 = it->second.find(SearchName);
		if (it2 == it->second.end()) return EBlendShapeName::None;

		return it2->second;
	}

	void CBlendShapeNameProvider::InitTable()
	{
		// 未対応
	}

	void CBlendShapeNameProvider::InitTableU16()
	{
		InitPMXTable();
		InitVMDTable();
	}

	void CBlendShapeNameProvider::InitPMXTable()
	{
		std::map<std::wstring, EBlendShapeName> BoneNameTable;

		// コードにUTF8以外の文字が含まれているとEmscriptenでコンパイルできないっぽいのでバイト列を実行時にwstringに変換している
		// また、Shift_JISからUTF16への変換が大変なので2番目にShift_JISのバイト列を追加している
		// ここに記載されているPmx標準シェイプを登録している
		// https://site.nicovideo.jp/ch/userblomaga_thanks/archive/ar1471249
		BoneNameTable.emplace(HexToWstr({ 0x771f, 0x9762, 0x76ee }), animation::EBlendShapeName::Neutral); // 真面目
		BoneNameTable.emplace(HexToWstr({ 0x307e, 0x3070, 0x305f, 0x304d }), animation::EBlendShapeName::Blink); // まばたき
		BoneNameTable.emplace(HexToWstr({ 0x30a6, 0x30a3, 0x30f3, 0x30af }), animation::EBlendShapeName::Blink_L); // ウィンク
		BoneNameTable.emplace(HexToWstr({ 0x30a6, 0x30a3, 0x30f3, 0x30af, 0x53f3 }), animation::EBlendShapeName::Blink_R); // ウィンク右
		BoneNameTable.emplace(HexToWstr({ 0x306b, 0x3053, 0x308a }), animation::EBlendShapeName::Joy); // にこり
		BoneNameTable.emplace(HexToWstr({ 0x6012, 0x308a }), animation::EBlendShapeName::Angry); // 怒り
		BoneNameTable.emplace(HexToWstr({ 0x56f0, 0x308b }), animation::EBlendShapeName::Sorrow); // 困る
		BoneNameTable.emplace(HexToWstr({ 0x7b11, 0x3044 }), animation::EBlendShapeName::Fun); // 笑い
		BoneNameTable.emplace(HexToWstr({ 0x3042 }), animation::EBlendShapeName::A); // あ
		BoneNameTable.emplace(HexToWstr({ 0x3044 }), animation::EBlendShapeName::I); // い
		BoneNameTable.emplace(HexToWstr({ 0x3046 }), animation::EBlendShapeName::U); // う
		BoneNameTable.emplace(HexToWstr({ 0x3048 }), animation::EBlendShapeName::E); // え
		BoneNameTable.emplace(HexToWstr({ 0x304a }), animation::EBlendShapeName::O); // お

		BoneNameTable.emplace(HexToWstr({ 0x4e0a }), animation::EBlendShapeName::Optional_Shape_0); // 上
		BoneNameTable.emplace(HexToWstr({ 0x4e0b }), animation::EBlendShapeName::Optional_Shape_1); // 下
		BoneNameTable.emplace(HexToWstr({ 0x30a6, 0x30a3, 0x30f3, 0x30af, 0xff12 }), animation::EBlendShapeName::Optional_Shape_2); // ウィンク２
		BoneNameTable.emplace(HexToWstr({ 0xff73, 0xff68, 0xff9d, 0xff78, 0xff12, 0x53f3 }), animation::EBlendShapeName::Optional_Shape_3); // ｳｨﾝｸ２右
		BoneNameTable.emplace(HexToWstr({ 0x306f, 0x3045 }), animation::EBlendShapeName::Optional_Shape_4); // はぅ
		BoneNameTable.emplace(HexToWstr({ 0x306a, 0x3054, 0x307f }), animation::EBlendShapeName::Optional_Shape_5); // なごみ
		BoneNameTable.emplace(HexToWstr({ 0x3073, 0x3063, 0x304f, 0x308a }), animation::EBlendShapeName::Optional_Shape_6); // びっくり
		BoneNameTable.emplace(HexToWstr({ 0x3058, 0x3068, 0x76ee }), animation::EBlendShapeName::Optional_Shape_7); // じと目
		BoneNameTable.emplace(HexToWstr({ 0x306a, 0x306c, 0xff01 }), animation::EBlendShapeName::Optional_Shape_8); // なぬ！
		BoneNameTable.emplace(HexToWstr({ 0x25b2 }), animation::EBlendShapeName::Optional_Shape_9); // ▲
		BoneNameTable.emplace(HexToWstr({ 0x2227 }), animation::EBlendShapeName::Optional_Shape_10); // ∧
		BoneNameTable.emplace(HexToWstr({ 0x03c9 }), animation::EBlendShapeName::Optional_Shape_11); // ω
		BoneNameTable.emplace(HexToWstr({ 0x03c9, 0x25a1 }), animation::EBlendShapeName::Optional_Shape_12); // ω□
		BoneNameTable.emplace(HexToWstr({ 0x306f, 0x3093, 0x3063, 0xff01 }), animation::EBlendShapeName::Optional_Shape_13); // はんっ！
		BoneNameTable.emplace(HexToWstr({ 0x3048, 0x30fc }), animation::EBlendShapeName::Optional_Shape_14); // えー
		BoneNameTable.emplace(HexToWstr({ 0x306b, 0x3084, 0x308a }), animation::EBlendShapeName::Optional_Shape_15); // にやり
		BoneNameTable.emplace(HexToWstr({ 0x77b3, 0x5c0f }), animation::EBlendShapeName::Optional_Shape_16); // 瞳小
		BoneNameTable.emplace(HexToWstr({ 0x307a, 0x308d, 0x3063 }), animation::EBlendShapeName::Optional_Shape_17); // ぺろっ

		m_TableListU16.emplace(EBonePattern::PMX, BoneNameTable);
	}

	void CBlendShapeNameProvider::InitVMDTable()
	{
		std::map<std::wstring, EBlendShapeName> BoneNameTable;

		// コードにUTF8以外の文字が含まれているとEmscriptenでコンパイルできないっぽいのでバイト列を実行時にwstringに変換している
		// また、Shift_JISからUTF16への変換が大変なので2番目にShift_JISのバイト列を追加している
		// ここに記載されているPmx標準シェイプを登録している
		// https://site.nicovideo.jp/ch/userblomaga_thanks/archive/ar1471249
		BoneNameTable.emplace(HexToWstr({ 0x905e, 0x96ca, 0x96da }), animation::EBlendShapeName::Neutral); // 真面目
		BoneNameTable.emplace(HexToWstr({ 0x82dc, 0x82ce, 0x82bd, 0x82ab }), animation::EBlendShapeName::Blink); // まばたき
		BoneNameTable.emplace(HexToWstr({ 0x8345, 0x8342, 0x8393, 0x834e }), animation::EBlendShapeName::Blink_L); // ウィンク
		BoneNameTable.emplace(HexToWstr({ 0x8345, 0x8342, 0x8393, 0x834e, 0x8945 }), animation::EBlendShapeName::Blink_R); // ウィンク右
		BoneNameTable.emplace(HexToWstr({ 0x82c9, 0x82b1, 0x82e8 }), animation::EBlendShapeName::Joy); // にこり
		BoneNameTable.emplace(HexToWstr({ 0x937b, 0x82e8 }), animation::EBlendShapeName::Angry); // 怒り
		BoneNameTable.emplace(HexToWstr({ 0x8da2, 0x82e9 }), animation::EBlendShapeName::Sorrow); // 困る
		BoneNameTable.emplace(HexToWstr({ 0x8fce, 0x82a2 }), animation::EBlendShapeName::Fun); // 笑い
		BoneNameTable.emplace(HexToWstr({ 0x82a0 }), animation::EBlendShapeName::A); // あ
		BoneNameTable.emplace(HexToWstr({ 0x82a2 }), animation::EBlendShapeName::I); // い
		BoneNameTable.emplace(HexToWstr({ 0x82a4 }), animation::EBlendShapeName::U); // う
		BoneNameTable.emplace(HexToWstr({ 0x82a6 }), animation::EBlendShapeName::E); // え
		BoneNameTable.emplace(HexToWstr({ 0x82a8 }), animation::EBlendShapeName::O); // お

		BoneNameTable.emplace(HexToWstr({ 0x8fe3 }), animation::EBlendShapeName::Optional_Shape_0); // 上
		BoneNameTable.emplace(HexToWstr({ 0x89ba }), animation::EBlendShapeName::Optional_Shape_1); // 下
		BoneNameTable.emplace(HexToWstr({ 0x8345, 0x8342, 0x8393, 0x834e, 0x8251 }), animation::EBlendShapeName::Optional_Shape_2); // ウィンク２
		BoneNameTable.emplace(HexToWstr({ 0xb3a8, 0xddb8, 0x8251, 0x8945 }), animation::EBlendShapeName::Optional_Shape_3); // ｳｨﾝｸ２右
		BoneNameTable.emplace(HexToWstr({ 0x82cd, 0x82a3 }), animation::EBlendShapeName::Optional_Shape_4); // はぅ
		BoneNameTable.emplace(HexToWstr({ 0x82c8, 0x82b2, 0x82dd }), animation::EBlendShapeName::Optional_Shape_5); // なごみ
		BoneNameTable.emplace(HexToWstr({ 0x82d1, 0x82c1, 0x82ad, 0x82e8 }), animation::EBlendShapeName::Optional_Shape_6); // びっくり
		BoneNameTable.emplace(HexToWstr({ 0x82b6, 0x82c6, 0x96da }), animation::EBlendShapeName::Optional_Shape_7); // じと目
		BoneNameTable.emplace(HexToWstr({ 0x82c8, 0x82ca, 0x8149 }), animation::EBlendShapeName::Optional_Shape_8); // なぬ！
		BoneNameTable.emplace(HexToWstr({ 0x81a3 }), animation::EBlendShapeName::Optional_Shape_9); // ▲
		BoneNameTable.emplace(HexToWstr({ 0x81c8 }), animation::EBlendShapeName::Optional_Shape_10); // ∧
		BoneNameTable.emplace(HexToWstr({ 0x83d6 }), animation::EBlendShapeName::Optional_Shape_11); // ω
		BoneNameTable.emplace(HexToWstr({ 0x83d6, 0x81a0 }), animation::EBlendShapeName::Optional_Shape_12); // ω□
		BoneNameTable.emplace(HexToWstr({ 0x82cd, 0x82f1, 0x82c1, 0x8149 }), animation::EBlendShapeName::Optional_Shape_13); // はんっ！
		BoneNameTable.emplace(HexToWstr({ 0x82a6, 0x815b }), animation::EBlendShapeName::Optional_Shape_14); // えー
		BoneNameTable.emplace(HexToWstr({ 0x82c9, 0x82e2, 0x82e8 }), animation::EBlendShapeName::Optional_Shape_15); // にやり
		BoneNameTable.emplace(HexToWstr({ 0x93b5, 0x8fac }), animation::EBlendShapeName::Optional_Shape_16); // 瞳小
		BoneNameTable.emplace(HexToWstr({ 0x82d8, 0x82eb, 0x82c1 }), animation::EBlendShapeName::Optional_Shape_17); // ぺろっ

		m_TableListU16.emplace(EBonePattern::VMD, BoneNameTable);
	}

	// HeplerFunc ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::wstring CBlendShapeNameProvider::HexToWstr(std::vector<int> byteArray)
	{
		std::wstring Dst = std::wstring();

		for (int byte : byteArray)
		{
			wchar_t w = *reinterpret_cast<const wchar_t*>(&byte);

			Dst.push_back(w);
		}

		return Dst;
	}

	std::wstring CBlendShapeNameProvider::ToNoZeroStr(const std::wstring& Src)
	{
		std::wstring Dst = std::wstring();

		for (auto c : Src)
		{
			if (c == '\0') break;

			Dst.push_back(c);
		}

		return Dst;
	}
}

#endif