#pragma once

#ifdef USE_WEBGPU
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "CWebGPUAPI.h"

#include "../../Graphics/CVertexBuffer.h"

namespace api
{
	class CWebGPUVertexBuffer : public graphics::CVertexBuffer
	{
		api::CWebGPUAPI* m_pGraphicsAPI;

		// Vertex
		int m_VertexCount;
		std::vector<WGPUBuffer> m_VertexBufferList;
		std::vector<size_t> m_VertexBufferSizeList;
	public:
		CWebGPUVertexBuffer(api::CWebGPUAPI* pGraphicsAPI);
		virtual ~CWebGPUVertexBuffer();

		const std::vector<WGPUBuffer>& GetVertexBufferList() const;
		const std::vector<size_t>& GetVertexBufferSizeList() const;

		virtual bool Create() override;
	};
}
#endif