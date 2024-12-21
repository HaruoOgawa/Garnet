#pragma once
#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include "CPresetPrimitive.h"

namespace api { class IGraphicsAPI; }

namespace graphics
{
	class IRenderer;
	class CMaterial;
	class CVertexBuffer;
	class CIndexBuffer;
	class CTextureSet;

	class CPrimitive
	{
		bool m_Enabled;

		graphics::EPresetPrimitiveType m_PresetType;

		std::shared_ptr<CVertexBuffer> m_VertexBuffer;
		std::shared_ptr<CIndexBuffer> m_IndexBuffer;

		std::vector<std::tuple<std::shared_ptr<graphics::IRenderer>, std::shared_ptr<CMaterial>>> m_RendererList;

		// モーフ
		bool m_UseMorph;
	public:
		CPrimitive(const std::shared_ptr<CVertexBuffer>& VertexBuffer, const std::shared_ptr<CIndexBuffer>& IndexBuffer);
		virtual ~CPrimitive();

		void SetEnabled(bool Flag);
		bool IsEnabled() const;

		void SetPresetType(graphics::EPresetPrimitiveType Type);
		graphics::EPresetPrimitiveType GetPresetType() const;

		void Release();

		bool Create(const std::vector<std::string>& PassNameList, const std::shared_ptr<graphics::CTextureSet>& TextureSet);

		bool Draw();

		const std::vector<std::tuple<std::shared_ptr<graphics::IRenderer>, std::shared_ptr<CMaterial>>>& GetRendererList() const;

		void AddMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CMaterial>& Material);

		const std::shared_ptr<CVertexBuffer>& GetVertexBuffer() const;

		// モーフ
		void SetUseMorph(bool Flag);
	};
}