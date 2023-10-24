#pragma once

// VRM用の拡張としてskeletonフィールドがあるようなのでひとまずSkeleton - BoneではなくglTFの仕様に沿って名称は Skin - Joint のペアにした

#include "CJoint.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace animation
{
	class CSkin
	{
		std::vector<std::shared_ptr<CJoint>> m_JointList;
		std::vector<glm::mat4> m_InverseBindMatrixList;
	public:
		CSkin();
		virtual ~CSkin();

		void AddJoint(const std::shared_ptr<CJoint>& Joint);
		void AddInverseBindMatrices(const std::vector<glm::mat4>& Matrices);

		bool CalcSkinMatrixList(std::vector<glm::mat4>& MatrixList);
	};
}