#ifdef USE_MMD
#include "CVMDData.h"
#include "../../Binary/CBinaryAnalyser.h"
#include "../../Debug/Message/Console.h"

namespace mmd
{
	CVMDData::CVMDData():
		m_MinFrameIndex(INT_MAX),
		m_MaxFrameIndex(INT_MIN)
	{
	}

	CVMDData::~CVMDData()
	{
	}

	const std::map<std::wstring, std::vector<SVMDFrame>>& CVMDData::GetFrameMap() const
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

	bool CVMDData::Analyse(const std::vector<unsigned char>& Data)
	{
		// Analyserを生成
		binary::CBinaryAnalyser Analyser(Data);

		// ヘッダ情報をチェック
		std::string header = "";
		if (!Analyser.GetString(header, 30)) return false;

		std::wstring modelName = L"";
		if (!Analyser.GetUTF16String(modelName, 20)) return false;

		// フレームデータ
		if (!AnalyseFrameData(Analyser)) return false;

		return true;
	}

	bool CVMDData::AnalyseFrameData(binary::CBinaryAnalyser& Analyser)
	{
		// フレームデータ数
		int FrameDataCount = 0;
		if (!Analyser.GetInt(FrameDataCount)) return false;

		for (int i = 0; i < FrameDataCount; i++)
		{
			// ボーン名
			std::wstring BoneName = L"";
			if (!Analyser.GetUTF16String(BoneName, 15)) return false;

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

			glm::vec2 X_Interpolation_A = glm::vec2(Analyser.GetFloat(), Analyser.GetFloat());
			glm::vec2 X_Interpolation_B = glm::vec2(Analyser.GetFloat(), Analyser.GetFloat());
			glm::vec2 Y_Interpolation_A = glm::vec2(Analyser.GetFloat(), Analyser.GetFloat());
			glm::vec2 Y_Interpolation_B = glm::vec2(Analyser.GetFloat(), Analyser.GetFloat());
			glm::vec2 Z_Interpolation_A = glm::vec2(Analyser.GetFloat(), Analyser.GetFloat());
			glm::vec2 Z_Interpolation_B = glm::vec2(Analyser.GetFloat(), Analyser.GetFloat());
			glm::vec2 R_Interpolation_A = glm::vec2(Analyser.GetFloat(), Analyser.GetFloat());
			glm::vec2 R_Interpolation_B = glm::vec2(Analyser.GetFloat(), Analyser.GetFloat());

			// MapにPairが無ければ新規作成
			if (m_FrameMap.find(BoneName) == m_FrameMap.end())
			{
				m_FrameMap.emplace(BoneName, std::vector<SVMDFrame>());
			}

			// Mapにデータを登録する
			SVMDFrame Frame = { BoneName, FrameIndex, Pos, Rot, X_Interpolation_A , X_Interpolation_B, Y_Interpolation_A , Y_Interpolation_B, Z_Interpolation_A , Z_Interpolation_B, R_Interpolation_A , R_Interpolation_B };

			m_FrameMap[BoneName].push_back(Frame);
		}

		return true;
	}
}
#endif