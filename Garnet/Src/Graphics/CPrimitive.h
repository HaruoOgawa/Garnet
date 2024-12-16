#pragma once
#include <memory>
#include <string>

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

		std::shared_ptr<graphics::IRenderer>				 m_Renderer;
		
		std::shared_ptr<CMaterial>							 m_Material;

		// モーフ
		bool m_UseMorph;
	public:
		CPrimitive(const std::shared_ptr<CVertexBuffer>& VertexBuffer, const std::shared_ptr<CIndexBuffer>& IndexBuffer, const std::shared_ptr<CMaterial>& Material);
		virtual ~CPrimitive();

		void SetEnabled(bool Flag);
		bool IsEnabled() const;

		void SetPresetType(graphics::EPresetPrimitiveType Type);
		graphics::EPresetPrimitiveType GetPresetType() const;

		void Release();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const std::string& PassName, const std::shared_ptr<graphics::CTextureSet>& TextureSet);

		bool Draw(int DynamicOffsetNum);

		const std::shared_ptr<graphics::IRenderer>& GetRenderer() const;

		void AddMaterial(const std::shared_ptr<CMaterial>& Material);
		const std::shared_ptr<CMaterial>& GetMaterial() const;

		const std::shared_ptr<CVertexBuffer>& GetVertexBuffer() const;

		// モーフ
		void SetUseMorph(bool Flag);
	};
}