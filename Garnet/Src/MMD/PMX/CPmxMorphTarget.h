#pragma once

#ifdef USE_MMD

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>

namespace mmd
{
	class CPmxMorphTarget
	{
		// モーフ名
		std::pair<std::string, std::wstring> m_MorphName;
		std::pair<std::string, std::wstring> m_MorphName_EN;

		// グループモーフ
		// 未実装

		// 頂点モーフ
		std::map<int, glm::vec3> m_VertexMorphList;

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
		void AddVertexMorph(int VertexIndex, const glm::vec3& Offset);
		const std::map<int, glm::vec3>& GetVertexMorphList() const;
	};
}
#endif