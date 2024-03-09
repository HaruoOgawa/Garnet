#pragma once

#ifdef USE_WEBGPU
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "CWebGPUAPI.h"

#include "../../Graphics/CIndexBuffer.h"

namespace graphics {
	enum class EIndiceType;
	enum class EDataType;

}

namespace api
{
	class CWebGPUIndexBuffer : public graphics::CIndexBuffer
	{
		api::CWebGPUAPI* m_pGraphicsAPI;

		// Index
		graphics::EIndiceType m_IndiceType;
		WGPUBuffer m_IndexBuffer;
		size_t     m_IndicesCount;
	public:
		CWebGPUIndexBuffer(api::CWebGPUAPI* pGraphicsAPI);
		virtual ~CWebGPUIndexBuffer();

		const WGPUBuffer& GetIndexBuffer() const;
		size_t GetIndicesCount() const;

		virtual bool Create() override;
	};
}
#endif