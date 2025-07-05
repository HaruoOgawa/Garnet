#pragma once
#include <memory>
#include <map>

namespace api { class IGraphicsAPI; }

namespace graphics
{
	class CVertexBuffer;
	class CIndexBuffer;

	enum class EPresetPrimitiveType
	{
		None = -1,

		BOARD = 0,
		CUBE = 1,
		POINT = 2,
		SPHERE = 3,
		CYLINDER = 4,

		Max = 5,
	};

	class CPresetPrimitive
	{
	public:
		static std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CreateFromType(api::IGraphicsAPI* pGraphicsAPI, graphics::EPresetPrimitiveType PresetType);

		static std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CreateBoard(api::IGraphicsAPI* pGraphicsAPI);
		static std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CreateBox(api::IGraphicsAPI* pGraphicsAPI);
		static std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CreatePoint(api::IGraphicsAPI* pGraphicsAPI);
		static std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CreateSphere(api::IGraphicsAPI* pGraphicsAPI);
		static std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CreateCylinder(api::IGraphicsAPI* pGraphicsAPI);
	};
}