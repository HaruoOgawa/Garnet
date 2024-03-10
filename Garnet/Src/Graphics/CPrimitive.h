#pragma once
#include <memory>
#include <string>

#include "CPresetPrimitive.h"

namespace graphics {
	class IRenderer;
}

namespace api { class IGraphicsAPI; }

namespace graphics
{
	class CMaterial;
	class CVertexBuffer;
	class CIndexBuffer;

	class CPrimitive
	{
		std::shared_ptr<CVertexBuffer> m_VertexBuffer;
		std::shared_ptr<CIndexBuffer> m_IndexBuffer;

		std::shared_ptr<graphics::IRenderer>				 m_Renderer;
		std::shared_ptr<graphics::IRenderer>				 m_DepthRenderer;
		
		int													 m_MaterialIndex;

		// モーフ
		bool m_UseMorph;
	public:
		CPrimitive(const std::shared_ptr<CVertexBuffer>& VertexBuffer, const std::shared_ptr<CIndexBuffer>& IndexBuffer, int MaterialIndex);
		virtual ~CPrimitive();

		void Release();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const std::string& PassName, const std::shared_ptr<graphics::CMaterial>& Material, bool IsDepth);

		bool Draw(const std::shared_ptr<CMaterial>& Material, int DynamicOffsetNum, bool IsDepth);

		const std::shared_ptr<graphics::IRenderer>& GetRenderer() const;

		void SetMaterialIndex(int Index);
		int GetMaterialIndex() const;

		const std::shared_ptr<CVertexBuffer>& GetVertexBuffer() const;

		// モーフ
		void SetUseMorph(bool Flag);
	};
}