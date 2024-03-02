#pragma once

#ifdef USE_MMD

#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace mmd
{
	struct SPmxVertexMorph
	{
		int VertexIndex = -1;
		glm::vec3 Offset = glm::vec3(0.0f);
	};

	class CPmxMorphTarget
	{
		// モーフ名
		std::pair<std::string, std::wstring> m_MorphName;
		std::pair<std::string, std::wstring> m_MorphName_EN;

		// グループモーフ
		// 未実装

		// 頂点モーフ
		std::vector<SPmxVertexMorph> m_VertexMorphList;

		// ボーンモーフ
		// 未実装

		// UVモーフ
		// 未実装

		// 追加UV1モーフ
		// 未実装

		// 追加UV2モーフ
		// 未実装

		// 追加UV3モーフ
		// 未実装

		// 追加UV4モーフ
		// 未実装

		// 材質モーフ
		// 未実装
	public:
		CPmxMorphTarget(const std::pair<std::string, std::wstring>& MorphName, const std::pair<std::string, std::wstring>& MorphName_EN);
		virtual ~CPmxMorphTarget() = default;

		// 頂点モーフ
		void AddVertexMorph(const SPmxVertexMorph& VertexMorph);
	};
}
#endif