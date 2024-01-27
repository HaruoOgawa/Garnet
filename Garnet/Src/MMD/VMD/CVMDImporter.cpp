#ifdef USE_MMD
#include "CVMDImporter.h"
#include "CVMDData.h"
#include "../../Binary/CBinaryAnalyser.h"
#include "../../Debug/Message/Console.h"

namespace mmd
{
	bool CVMDImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList)
	{
		CVMDData vmd;

		// バイナリの解析
		if (!vmd.Analyse(Data))
		{
			Console::Log("[Cpp Log] Error - Failed to Analyse VMD Binary.\n");

			return false;
		}

		// Analyserを生成
		binary::CBinaryAnalyser Analyser(Data);

		// ヘッダ情報をチェック
		std::string header = "";
		if (!Analyser.GetString(header, 30)) return false;

		std::string modelName = "";
		if (!Analyser.GetString(modelName, 20)) return false;

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
		}

		return true;
	}
}
#endif