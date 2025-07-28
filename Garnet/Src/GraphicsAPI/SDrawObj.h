#pragma once

#include <memory>
#include <tuple>
#include <glm/glm.hpp>

namespace graphics
{
	class IRenderer;
	class CMaterial;
	class CVertexBuffer;
	class CIndexBuffer;
}

namespace api
{
	struct SDrawObj
	{
		int RenderQueue = 2000;
		float ToCameraDist = 0.0f;
		std::tuple<std::shared_ptr<graphics::IRenderer>, std::shared_ptr<graphics::CMaterial>> RendererMat = std::make_tuple(nullptr, nullptr);
		std::shared_ptr<graphics::CVertexBuffer> VertexBuffer = nullptr;
		std::shared_ptr<graphics::CIndexBuffer> IndexBuffer = nullptr;
		glm::mat4 WorldMatrix = glm::mat4(1.0f);
	};
}