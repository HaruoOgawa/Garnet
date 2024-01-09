#ifdef USE_MMD
#include "CPmxModel.h"
#include "../../Binary/CBinaryAnalyser.h"

namespace mmd
{
	bool CPmxModel::Analyse(const std::vector<unsigned char>& Data)
	{
		// Analyserを生成
		binary::CBinaryAnalyser Analyser(Data);

		// ヘッダが『PMX 』かどうか
		std::string header = "";
		if (!Analyser.GetString(header, 4)) return false;
		if (header != "PMX ") return false;

		// Version
		if (Analyser.GetPointer()[0] != 0x00 || Analyser.GetPointer()[1] != 0x00 || Analyser.GetPointer()[2] != 0x00 || Analyser.GetPointer()[3] != 0x40) return false;
		if (!Analyser.Skip(4)) return false;

		// メタデータ
		SMetaData MetaData;
		if (!AnalyseMetaData(Analyser, MetaData)) return false;

		// Mesh
		if (!AnalyseMesh(Analyser)) return false;

		return true;
	}

	bool CPmxModel::AnalyseMetaData(binary::CBinaryAnalyser& Analyser, SMetaData& MetaData)
	{
		// 後続のメタデータの長さ(PMX 2.0では8に固定)
		unsigned char MetaSize = 0;
		if (!Analyser.GetByte(MetaSize)) return false;

		if (!Analyser.IsValid(static_cast<size_t>(MetaSize))) return false;

		// メタデータを読む
		MetaData.EncodeType = static_cast<EEncodeType>(static_cast<int>(Analyser.GetByte())); // エンコード方式
		MetaData.AdditionalUVCount = static_cast<int>(Analyser.GetByte()); // 追加UV数
		MetaData.VertexIndexSize = static_cast<int>(Analyser.GetByte()); // 頂点インデックスサイズ
		MetaData.TextureIndexSize = static_cast<int>(Analyser.GetByte()); // テクスチャインデックスサイズ
		MetaData.MaterialIndexSize = static_cast<int>(Analyser.GetByte()); // マテリアルインデックスサイズ
		MetaData.BoneIndexSize = static_cast<int>(Analyser.GetByte()); // ボーンインデックスサイズ
		MetaData.MorphIndexSize = static_cast<int>(Analyser.GetByte()); // モーフインデックスサイズ
		MetaData.RigidIndexSize = static_cast<int>(Analyser.GetByte()); // 剛体インデックスサイズ

		// モデル名
		{
			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			std::string ModelName;
			if (MetaData.EncodeType == EEncodeType::UTF16)
			{
				//if (!Analyser.GetUTF16String(ModelName, ByteLength)) return false;

				// ひとまずUTF8で読んでおく
				if (!Analyser.GetString(ModelName, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EEncodeType::UTF8)
			{
				if (!Analyser.GetString(ModelName, ByteLength)) return false;
			}

			MetaData.ModelName = ModelName;
		}

		// モデル名英
		{
			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			std::string ModelName;
			if (MetaData.EncodeType == EEncodeType::UTF16)
			{
				//if (!Analyser.GetUTF16String(ModelName, ByteLength)) return false;

				// ひとまずUTF8で読んでおく
				if (!Analyser.GetString(ModelName, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EEncodeType::UTF8)
			{
				if (!Analyser.GetString(ModelName, ByteLength)) return false;
			}

			MetaData.ModelName_EN = ModelName;
		}

		// コメント
		{
			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			std::string Comment;
			if (MetaData.EncodeType == EEncodeType::UTF16)
			{
				//if (!Analyser.GetUTF16String(Comment, ByteLength)) return false;

				// ひとまずUTF8で読んでおく
				if (!Analyser.GetString(Comment, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EEncodeType::UTF8)
			{
				if (!Analyser.GetString(Comment, ByteLength)) return false;
			}

			MetaData.Comment = Comment;
		}

		// コメント英
		{
			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			std::string Comment_EN;
			if (MetaData.EncodeType == EEncodeType::UTF16)
			{
				//if (!Analyser.GetUTF16String(Comment_EN, ByteLength)) return false;

				// ひとまずUTF8で読んでおく
				if (!Analyser.GetString(Comment_EN, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EEncodeType::UTF8)
			{
				if (!Analyser.GetString(Comment_EN, ByteLength)) return false;
			}

			MetaData.Comment_EN = Comment_EN;
		}

		return true;
	}

	bool CPmxModel::AnalyseMesh(binary::CBinaryAnalyser& Analyser)
	{
		int NumOfVertex = 0;
		if (!Analyser.GetInt(NumOfVertex)) return false;

		return true;
	}
}
#endif