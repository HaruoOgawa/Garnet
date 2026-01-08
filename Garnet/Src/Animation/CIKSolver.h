#pragma once

#ifdef USE_ANIMATION

#include <glm/glm.hpp>
#include <vector>
#include <tuple>
#include <memory>

#include "../Object/CNode.h"

namespace animation
{
	class CBone;
	struct SIKParam;

	class CIKSolver
	{
		std::shared_ptr<animation::SIKParam> m_IKParam;

		std::vector<std::shared_ptr<object::CNode>> m_IKChainList;

		std::shared_ptr<object::CNode> m_IKTarget;
	public:
		CIKSolver();
		virtual ~CIKSolver();

		const std::vector<std::shared_ptr<object::CNode>>& GetIKChainList() const;

		bool Create(const std::shared_ptr<CBone>& IKTargetBone, const std::vector<std::tuple<std::string, std::shared_ptr<CBone>>>& BoneList);

		bool Solve();
	};
}
#endif