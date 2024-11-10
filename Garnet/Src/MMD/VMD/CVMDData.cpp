#if defined(USE_MMD)  && defined(USE_BINARY_READ)
#include "CVMDData.h"
#include "../../Binary/CBinaryReader.h"
#include "../../Message/Console.h"
#include "../../Animation/CBoneNameProvider.h"
#include "../../Animation/CBlendShapeNameProvider.h"

namespace mmd
{
	CVMDData::CVMDData():
		m_MinFrameIndex(INT_MAX),
		m_MaxFrameIndex(INT_MIN),
		m_MinSkinFrameIndex(INT_MAX),
		m_MaxSkinFrameIndex(INT_MIN)
	{
	}

	CVMDData::~CVMDData()
	{
	}

	// ボーンアニメーション
	const std::map<animation::EHumanoidBones, std::vector<SVMDFrame>>& CVMDData::GetFrameMap() const
	{
		return m_FrameMap;
	}

	int CVMDData::GetMinFrameIndex() const
	{
		return m_MinFrameIndex;
	}

	int CVMDData::GetMaxFrameIndex() const
	{
		return m_MaxFrameIndex;
	}

	// 表情アニメーション
	const std::map<animation::EBlendShapeName, std::vector<SVMDSkinFrame>>& CVMDData::GetSkinFrameMap() const
	{
		return m_SkinFrameMap;
	}

	int CVMDData::GetMinSkinFrameIndex() const
	{
		return m_MinSkinFrameIndex;
	}

	int CVMDData::GetMaxSkinFrameIndex() const
	{
		return m_MaxSkinFrameIndex;
	}

	bool CVMDData::Analyse(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data)
	{
		// Analyserを生成
		binary::CBinaryReader Analyser(Data);

		/*
		// ヘッダ
		struct VMD_HEADER {
		char VmdHeader[30]; // "Vocaloid Motion Data 0002"
		char VmdModelName[20]; // カメラの場合:"カメラ・照明" // カメラ・照明・アクセサリモードではモデル用のVMDは読めなくなりました(7.10-)
		} vmd_header;
		*/

		// ヘッダ情報をチェック
		std::string header = "";
		if (!Analyser.GetString(header, 30)) return false;

		std::wstring modelName = L"";
		if (!Analyser.GetUTF16ReverseString(modelName, 20)) return false;

		// フレームデータ
		if (!AnalyseFrameData(pGraphicsAPI, Analyser)) return false;

		// 表情データ
		// スキンデータと書かれることが多いがこれはリターゲット用のスキンデータではなく表情という意味らしい
		// (なのでVMDモーションはリターゲット不要と捉えていいのかな？)
		if (!AnalyseFacialExpressionData(pGraphicsAPI, Analyser)) return false;

		// カメラデータ
		if (!AnalyseCameraData(Analyser)) return false;

		// 照明データ
		if (!AnalyseLightData(Analyser)) return false;

		// セルフシャドウデータ
		if (!AnalyseSelfShadowData(Analyser)) return false;

		return true;
	}

	bool CVMDData::AnalyseFrameData(api::IGraphicsAPI* pGraphicsAPI, binary::CBinaryReader& Analyser)
	{
		/*
		// モーションデータ数
		//
		// キーフレーム数の上限(MMD側で制限):300,000(32bit版), 600,000(64bit版 7.39dot-)
		// 上限判定時は、フレーム0にあるキー(デフォルトではモデルのボーン数と同数)もカウントされるので注意
		//
		struct VMD_MOTION_COUNT {
		DWORD Count;// モーションデータ数
		} vmd_motion_count;

		// モーションデータ
		struct VMD_MOTION { // 111 Bytes // モーション
		char BoneName[15]; // ボーン名
		DWORD FrameNo; // フレーム番号(読込時は現在のフレーム位置を0とした相対位置)
		float Location[3]; // 位置
		float Rotatation[4]; // Quaternion // 回転
		BYTE Interpolation[64]; // [4][4][4] // 補完
		} vmd_motion;
		*/

		// フレームデータ数
		int FrameDataCount = 0;
		if (!Analyser.GetInt(FrameDataCount)) return false;

		for (int i = 0; i < FrameDataCount; i++)
		{
			// ボーン名
			std::wstring Name = L"";
			if (!Analyser.GetUTF16ReverseString(Name, 15)) return false;

			// フレームインデックス
			int FrameIndex = -1;
			if (!Analyser.GetInt(FrameIndex)) return false;

			m_MinFrameIndex = std::min(FrameIndex, m_MinFrameIndex);
			m_MaxFrameIndex = std::max(FrameIndex, m_MaxFrameIndex);

			// ボーンの位置
			if (!Analyser.IsValid(4 * 3)) return false;

			glm::vec3 Pos = glm::vec3(0.0f);

			Pos.x = Analyser.GetFloat();
			Pos.y = Analyser.GetFloat();
			Pos.z = Analyser.GetFloat();

			// ボーンの回転
			if (!Analyser.IsValid(4 * 4)) return false;

			glm::quat Rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

			Rot.x = Analyser.GetFloat();
			Rot.y = Analyser.GetFloat();
			Rot.z = Analyser.GetFloat();
			Rot.w = Analyser.GetFloat();

			// 補完パラメーター(ベジュ曲線に使用する) - Interpolation Params
			if (!Analyser.IsValid(4 * 4 * 4)) return false;

			//
			glm::vec2 X_Interpolation_A = glm::vec2(0.0f);
			glm::vec2 X_Interpolation_B = glm::vec2(0.0f);
			glm::vec2 Y_Interpolation_A = glm::vec2(0.0f);
			glm::vec2 Y_Interpolation_B = glm::vec2(0.0f);
			glm::vec2 Z_Interpolation_A = glm::vec2(0.0f);
			glm::vec2 Z_Interpolation_B = glm::vec2(0.0f);
			glm::vec2 R_Interpolation_A = glm::vec2(0.0f);
			glm::vec2 R_Interpolation_B = glm::vec2(0.0f);

			// ax
			{
				X_Interpolation_A.x = static_cast<float>(Analyser.GetByte());
				Y_Interpolation_A.x = static_cast<float>(Analyser.GetByte());
				Z_Interpolation_A.x = static_cast<float>(Analyser.GetByte());
				R_Interpolation_A.x = static_cast<float>(Analyser.GetByte());
			}

			// ay
			{
				X_Interpolation_A.y = static_cast<float>(Analyser.GetByte());
				Y_Interpolation_A.y = static_cast<float>(Analyser.GetByte());
				Z_Interpolation_A.y = static_cast<float>(Analyser.GetByte());
				R_Interpolation_A.y = static_cast<float>(Analyser.GetByte());
			}

			// bx
			{
				X_Interpolation_B.x = static_cast<float>(Analyser.GetByte());
				Y_Interpolation_B.x = static_cast<float>(Analyser.GetByte());
				Z_Interpolation_B.x = static_cast<float>(Analyser.GetByte());
				R_Interpolation_B.x = static_cast<float>(Analyser.GetByte());
			}

			// by
			{
				X_Interpolation_B.y = static_cast<float>(Analyser.GetByte());
				Y_Interpolation_B.y = static_cast<float>(Analyser.GetByte());
				Z_Interpolation_B.y = static_cast<float>(Analyser.GetByte());
				R_Interpolation_B.y = static_cast<float>(Analyser.GetByte());
			}

			//
			std::vector<glm::vec2> XPointList;
			{
				XPointList.push_back(glm::vec2(0.0f, 0.0f));
				XPointList.push_back(X_Interpolation_A / 127.0f);
				XPointList.push_back(X_Interpolation_B / 127.0f);
				XPointList.push_back(glm::vec2(1.0f, 1.0f));
			}

			std::vector<glm::vec2> YPointList;
			{
				YPointList.push_back(glm::vec2(0.0f, 0.0f));
				YPointList.push_back(Y_Interpolation_A / 127.0f);
				YPointList.push_back(Y_Interpolation_B / 127.0f);
				YPointList.push_back(glm::vec2(1.0f, 1.0f));
			}

			std::vector<glm::vec2> ZPointList;
			{
				ZPointList.push_back(glm::vec2(0.0f, 0.0f));
				ZPointList.push_back(Z_Interpolation_A / 127.0f);
				ZPointList.push_back(Z_Interpolation_B / 127.0f);
				ZPointList.push_back(glm::vec2(1.0f, 1.0f));
			}

			std::vector<glm::vec2> RPointList;
			{
				RPointList.push_back(glm::vec2(0.0f, 0.0f));
				RPointList.push_back(R_Interpolation_A / 127.0f);
				RPointList.push_back(R_Interpolation_B / 127.0f);
				RPointList.push_back(glm::vec2(1.0f, 1.0f));
			}

			// 残りの48バイトはひとまずスキップ
			if (!Analyser.Skip(16 * 3)) return false;

			// ボーン名を取得
			animation::EHumanoidBones BoneName = pGraphicsAPI->GetBoneNameProvider()->GetBoneNameU16(animation::EBonePattern::VMD, Name);

			// Noneはどのボーンに割り当てればいいかわからないのでスキップする
			if (BoneName == animation::EHumanoidBones::None) continue;

			// MapにPairが無ければ新規作成
			if (m_FrameMap.find(BoneName) == m_FrameMap.end())
			{
				m_FrameMap.emplace(BoneName, std::vector<SVMDFrame>());
			}

			// Mapにデータを登録する
			SVMDFrame Frame = { BoneName, FrameIndex, Pos, Rot, XPointList, YPointList, ZPointList, RPointList };

			m_FrameMap[BoneName].push_back(Frame);
		}

		return true;
	}

	bool CVMDData::AnalyseFacialExpressionData(api::IGraphicsAPI* pGraphicsAPI, binary::CBinaryReader& Analyser)
	{
		/*
		// 表情データ数
		struct VMD_Skeleton_COUNT {
		DWORD Count; // 表情データ数
		} vmd_Skeleton_count;

		// 表情データ
		struct VMD_Skeleton { // 23 Bytes // 表情
		char SkeletonName[15]; // 表情名
		DWORD FlameNo; // フレーム番号
		float Weight; // 表情の設定値(表情スライダーの値)
		} vmd_Skeleton;
		*/

		// 表情データ数
		int ExpressionCount = 0;
		if (!Analyser.GetInt(ExpressionCount)) return false;

		for (int i = 0; i < ExpressionCount; i++)
		{
			// 表情名
			std::wstring Name = std::wstring(L"");
			if (!Analyser.GetUTF16ReverseString(Name, 15)) return false;

			// 同じ『まばたき』の文字列でもなぜかwstringのバイナリ上では途中にDとか)が入ってmapとしては別のものとして扱われてしまうようなのでVmdのパース段階で分ける必要がある
			// (本当はBlendName数 * ExpressionCountだけロードに時間がかかってしまうのであまりやりたくはないが・・・)
			// Boneの方も同じ理屈でVmdパース時にHumanoidBoneNameを見ている
			animation::EBlendShapeName BlendShapeName = pGraphicsAPI->GetBlendShapeNameProvider()->GetBlendShapeNameU16(Name);
			
			// なぜかNoneチェックをしているとNoneではないものも飛ばされてしまうのでひとまずコメントアウトしている(Blinkが16個あるはずなのになぜか4つとかになっていた)
			//if (BlendShapeName == animation::EBlendShapeName::None) continue;

			// フレームインデックス
			int FrameIndex = -1;
			if (!Analyser.GetInt(FrameIndex)) return false;

			m_MinSkinFrameIndex = std::min(FrameIndex, m_MinSkinFrameIndex);
			m_MaxSkinFrameIndex = std::max(FrameIndex, m_MaxSkinFrameIndex);

			// ウェイト
			float Weight = 0.0f;
			if (!Analyser.GetFloat(Weight)) return false;

			// 登録
			if (m_SkinFrameMap.find(BlendShapeName) == m_SkinFrameMap.end())
			{
				m_SkinFrameMap.emplace(BlendShapeName, std::vector<SVMDSkinFrame>());
			}

			SVMDSkinFrame SkinFrame = { FrameIndex ,Weight };
			m_SkinFrameMap[BlendShapeName].push_back(SkinFrame);
		}

		return true;
	}

	bool CVMDData::AnalyseCameraData(binary::CBinaryReader& Analyser)
	{
		/*
		// カメラデータ数
		struct VMD_CAMERA_COUNT {
		DWORD Count; // カメラデータ数
		} vmd_camera_count;

		// カメラデータ
		struct VMD_CAMERA { // 61 Bytes // カメラ
		DWORD FlameNo; // フレーム番号
		float Length; // -(距離)
		float Location[3]; // 位置
		float Rotation[3]; // オイラー角 // X軸は符号が反転しているので注意 // 回転
		BYTE Interpolation[24]; // おそらく[6][4](未検証) // 補完
		DWORD ViewingAngle; // 視界角
		BYTE Perspective; // 0:on 1:off // パースペクティブ
		} vmd_camera;
		*/

		return true;
	}

	bool CVMDData::AnalyseLightData(binary::CBinaryReader& Analyser)
	{
		/*
		// 照明データ数
		struct VMD_LIGHT_COUNT {
		DWORD Count; // 照明データ数
		} vmd_light_count;

		// 照明データ
		struct VMD_LIGHT { // 28 Bytes // 照明
		DWORD FlameNo; // フレーム番号
		float RGB[3]; // RGB各値/256 // 赤、緑、青
		float Location[3]; // X, Y, Z
		} vmd_light;
		*/

		return true;
	}

	bool CVMDData::AnalyseSelfShadowData(binary::CBinaryReader& Analyser)
	{
		/*
		// セルフシャドウデータ数
		struct VMD_SELF_SHADOW_COUNT {
		DWORD Count; // セルフシャドウデータ数
		} vmd_self_shadow_count;

		// セルフシャドウデータ
		struct VMD_SELF_SHADOW { // 9 Bytes // セルフシャドー
		DWORD FlameNo; // フレーム番号
		BYTE Mode; // 00-02 // モード
		float Distance; // 0.1 - (dist * 0.00001) // 距離
		} vmd_self_shadow;
		*/

		return true;
	}
}
#endif