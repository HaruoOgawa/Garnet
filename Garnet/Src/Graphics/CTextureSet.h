#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <tuple>
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
		std::map<std::string, std::vector<std::shared_ptr<graphics::CTexture>>> m_FrameTextureMap;

		// TEXTURE_IBL
		std::shared_ptr<graphics::CTexture> m_IBL_Diffuse_Tex;
		std::shared_ptr<graphics::CTexture> m_IBL_Specular_Tex;
		std::shared_ptr<graphics::CTexture> m_IBL_GGXLUT_Tex;
	public:
		CTextureSet();
		CTextureSet(const std::shared_ptr<graphics::CTexture>& Texture);
		CTextureSet(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, 
			const std::vector<std::shared_ptr<graphics::CTexture>>& CubeMapList, 
			const std::map<std::string, std::vector<std::shared_ptr<graphics::CTexture>>>& FrameTextureMap,
			const std::shared_ptr<graphics::CTexture>& IBL_Diffuse_Tex, 
			const std::shared_ptr<graphics::CTexture>& IBL_Specular_Tex, 
			const std::shared_ptr<graphics::CTexture>& IBL_GGXLUT_Tex);
		virtual ~CTextureSet();

		void Add2DTexture(const std::shared_ptr<graphics::CTexture>& Texture);
		const std::vector<std::shared_ptr<graphics::CTexture>>& Get2DTextureList() const;

		void AddCubeMap(const std::shared_ptr<graphics::CTexture>& CubeMap);
		const std::vector<std::shared_ptr<graphics::CTexture>>& GetCubeMapList() const;

		void AddFrameTexture(const std::string& PassName, const std::shared_ptr<graphics::CTexture>& FrameTexture);
		const std::map<std::string, std::vector<std::shared_ptr<graphics::CTexture>>>& GetFrameTextureMap() const;

		void AddIBLTexture(const std::shared_ptr<graphics::CTexture>& IBL_Diffuse_Tex, const std::shared_ptr<graphics::CTexture>& IBL_Specular_Tex, const std::shared_ptr<graphics::CTexture>& IBL_GGXLUT_Tex);
		const std::shared_ptr<graphics::CTexture>& GetDiffuse_Tex() const;
		const std::shared_ptr<graphics::CTexture>& GetSpecular_Tex() const;
		const std::shared_ptr<graphics::CTexture>& GetGGXLUT_Tex() const;
	};
}