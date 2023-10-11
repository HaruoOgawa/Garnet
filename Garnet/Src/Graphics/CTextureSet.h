#pragma once
#include <memory>
#include <vector>
#include "../Graphics/CTexture.h"

// テクスチャの受け渡しは全てここに集約する
namespace graphics
{	
	class CTextureSet
	{
		// TEXTURE_2D
		std::vector<std::shared_ptr<graphics::CTexture>> m_2DTextureList;
		
		// TEXTURE_CUBE
		std::vector<std::shared_ptr<graphics::CTexture>> m_CubeMapList;

		// TEXTURE_FRAME
		std::vector<std::shared_ptr<graphics::CTexture>> m_FrameTextureList;

		// TEXTURE_IBL
		std::shared_ptr<graphics::CTexture> m_IBL_Diffuse_Tex;
		std::shared_ptr<graphics::CTexture> m_IBL_Specular_Tex;
		std::shared_ptr<graphics::CTexture> m_BL_GGXLUT_Tex;
	public:
		CTextureSet();
		virtual ~CTextureSet();

		void Add2DTexture(const std::shared_ptr<graphics::CTexture>& Texture);
		const std::vector<std::shared_ptr<graphics::CTexture>>& Get2DTextureList() const;

		void AddCubeMap(const std::shared_ptr<graphics::CTexture>& CubeMap);
		const std::vector<std::shared_ptr<graphics::CTexture>>& GetCubeMapList() const;

		void AddFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture);
		const std::vector<std::shared_ptr<graphics::CTexture>>& GetFrameTextureList() const;

		void AddIBLTexture(const std::shared_ptr<graphics::CTexture>& IBL_Diffuse_Tex, const std::shared_ptr<graphics::CTexture>& IBL_Specular_Tex, const std::shared_ptr<graphics::CTexture>& IBL_GGXLUT_Tex);
		const std::shared_ptr<graphics::CTexture>& GetDiffuse_Tex() const;
		const std::shared_ptr<graphics::CTexture>& GetSpecular_Tex() const;
		const std::shared_ptr<graphics::CTexture>& GetGGXLUT_Tex() const;
	};
}