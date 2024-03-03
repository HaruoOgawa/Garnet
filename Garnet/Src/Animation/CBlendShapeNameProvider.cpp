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

	EBlendShapeName CBlendShapeNameProvider::GetBlendShapeNameU16(const std::wstring& SrcNodeName)
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

		for (const auto& Table : m_BlendShapeNameTableU16)
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
						const nameCount = $3;

						return (w0 == w1[windex] && w1.length == nameCount) ? 1 : 0;
						}, & BoneName[windex], & SearchName[0], windex, static_cast<int>(BoneName.size()));

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
					/*int Flag = EM_ASM_INT({
						const w0 = Module.UTF16ToString($0);

						return (w0 != "" && w0 != " ") ? 1 : 0;
					}, &SearchName[0]);

					if (Flag == 0)
					{
						Result = false;
					}*/
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

		return EBlendShapeName::None;
	}

	void CBlendShapeNameProvider::InitTable()
	{
		// 未対応
	}

	void CBlendShapeNameProvider::InitTableU16()
	{
		// コードにUTF8以外の文字が含まれているとEmscriptenでコンパイルできないっぽいのでバイト列を実行時にwstringに変換している
		// また、Shift_JISからUTF16への変換が大変なので2番目にShift_JISのバイト列を追加している
		// ここに記載されているPmx標準シェイプを登録している
		// https://site.nicovideo.jp/ch/userblomaga_thanks/archive/ar1471249
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Neutral, std::vector<std::wstring>{ HexToWstr({ 0x771f, 0x9762, 0x76ee }), HexToWstr({ 0x905e, 0x96ca, 0x96da }) }); // 真面目
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Blink, std::vector<std::wstring>{ HexToWstr({ 0x307e, 0x3070, 0x305f, 0x304d }), HexToWstr({ 0x82dc, 0x82ce, 0x82bd, 0x82ab }) }); // まばたき
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Blink_L, std::vector<std::wstring>{ HexToWstr({ 0x30a6, 0x30a3, 0x30f3, 0x30af }), HexToWstr({ 0x8345, 0x8342, 0x8393, 0x834e }) }); // ウィンク
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Blink_R, std::vector<std::wstring>{ HexToWstr({ 0x30a6, 0x30a3, 0x30f3, 0x30af, 0x53f3 }), HexToWstr({ 0x8345, 0x8342, 0x8393, 0x834e, 0x8945 }) }); // ウィンク右
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Joy, std::vector<std::wstring>{ HexToWstr({ 0x306b, 0x3053, 0x308a }), HexToWstr({ 0x82c9, 0x82b1, 0x82e8 }) }); // にこり
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Angry, std::vector<std::wstring>{ HexToWstr({ 0x6012, 0x308a }), HexToWstr({ 0x937b, 0x82e8 }) }); // 怒り
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Sorrow, std::vector<std::wstring>{ HexToWstr({ 0x56f0, 0x308b }), HexToWstr({ 0x8da2, 0x82e9 }) }); // 困る
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Fun, std::vector<std::wstring>{ HexToWstr({ 0x7b11, 0x3044 }), HexToWstr({ 0x8fce, 0x82a2 }) }); // 笑い
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::A, std::vector<std::wstring>{ HexToWstr({ 0x3042 }), HexToWstr({ 0x82a0 }) }); // あ
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::I, std::vector<std::wstring>{ HexToWstr({ 0x3044 }), HexToWstr({ 0x82a2 }) }); // い
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::U, std::vector<std::wstring>{ HexToWstr({ 0x3046 }), HexToWstr({ 0x82a4 }) }); // う
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::E, std::vector<std::wstring>{ HexToWstr({ 0x3048 }), HexToWstr({ 0x82a6 }) }); // え
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::O, std::vector<std::wstring>{ HexToWstr({ 0x304a }), HexToWstr({ 0x82a8 }) }); // お

		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_0, std::vector<std::wstring>{ HexToWstr({ 0x4e0a }), HexToWstr({ 0x8fe3 }) }); // 上
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_1, std::vector<std::wstring>{ HexToWstr({ 0x4e0b }), HexToWstr({ 0x89ba }) }); // 下
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_2, std::vector<std::wstring>{ HexToWstr({ 0x30a6, 0x30a3, 0x30f3, 0x30af, 0xff12 }), HexToWstr({ 0x8345, 0x8342, 0x8393, 0x834e, 0x8251 }) }); // ウィンク２
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_3, std::vector<std::wstring>{ HexToWstr({ 0xff73, 0xff68, 0xff9d, 0xff78, 0xff12, 0x53f3 }), HexToWstr({ 0xb3a8, 0xddb8, 0x8251, 0x8945 }) }); // ｳｨﾝｸ２右
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_4, std::vector<std::wstring>{ HexToWstr({ 0x306f, 0x3045 }), HexToWstr({ 0x82cd, 0x82a3 }) }); // はぅ
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_5, std::vector<std::wstring>{ HexToWstr({ 0x306a, 0x3054, 0x307f }), HexToWstr({ 0x82c8, 0x82b2, 0x82dd }) }); // なごみ
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_6, std::vector<std::wstring>{ HexToWstr({ 0x3073, 0x3063, 0x304f, 0x308a }), HexToWstr({ 0x82d1, 0x82c1, 0x82ad, 0x82e8 }) }); // びっくり
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_7, std::vector<std::wstring>{ HexToWstr({ 0x3058, 0x3068, 0x76ee }), HexToWstr({ 0x82b6, 0x82c6, 0x96da }) }); // じと目
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_8, std::vector<std::wstring>{ HexToWstr({ 0x306a, 0x306c, 0xff01 }), HexToWstr({ 0x82c8, 0x82ca, 0x8149 }) }); // なぬ！
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_9, std::vector<std::wstring>{ HexToWstr({ 0x25b2 }), HexToWstr({ 0x81a3 }) }); // ▲
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_10, std::vector<std::wstring>{ HexToWstr({ 0x2227 }), HexToWstr({ 0x81c8 }) }); // ∧
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_11, std::vector<std::wstring>{ HexToWstr({ 0x03c9 }), HexToWstr({ 0x83d6 }) }); // ω
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_12, std::vector<std::wstring>{ HexToWstr({ 0x03c9, 0x25a1 }), HexToWstr({ 0x83d6, 0x81a0 }) }); // ω□
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_13, std::vector<std::wstring>{ HexToWstr({ 0x306f, 0x3093, 0x3063, 0xff01 }), HexToWstr({ 0x82cd, 0x82f1, 0x82c1, 0x8149 }) }); // はんっ！
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_14, std::vector<std::wstring>{ HexToWstr({ 0x3048, 0x30fc }), HexToWstr({ 0x82a6, 0x815b }) }); // えー
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_15, std::vector<std::wstring>{ HexToWstr({ 0x306b, 0x3084, 0x308a }), HexToWstr({ 0x82c9, 0x82e2, 0x82e8 }) }); // にやり
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_16, std::vector<std::wstring>{ HexToWstr({ 0x77b3, 0x5c0f }), HexToWstr({ 0x93b5, 0x8fac }) }); // 瞳小
		m_BlendShapeNameTableU16.emplace(animation::EBlendShapeName::Optional_Shape_17, std::vector<std::wstring>{ HexToWstr({ 0x307a, 0x308d, 0x3063 }), HexToWstr({ 0x82d8, 0x82eb, 0x82c1 }) }); // ぺろっ
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
}

#endif