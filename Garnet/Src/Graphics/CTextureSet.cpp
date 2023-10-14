#include "CTextureSet.h"

// テクスチャの受け渡しは全てここに集約する
namespace graphics
{
	CTextureSet::CTextureSet():
		m_IBL_Diffuse_Tex(nullptr),
		m_IBL_Specular_Tex(nullptr),
		m_BL_GGXLUT_Tex(nullptr)
	{
	}

	CTextureSet::~CTextureSet()
	{
	}

	void CTextureSet::Add2DTexture(const std::shared_ptr<graphics::CTexture>& Texture)
	{
		m_2DTextureList.push_back(Texture);
	}

	const std::vector<std::shared_ptr<graphics::CTexture>>& CTextureSet::Get2DTextureList() const
	{
		return m_2DTextureList;
	}

	void CTextureSet::AddCubeMap(const std::shared_ptr<graphics::CTexture>& CubeMap)
	{
		m_CubeMapList.push_back(CubeMap);
	}

	const std::vector<std::shared_ptr<graphics::CTexture>>& CTextureSet::GetCubeMapList() const
	{
		return m_CubeMapList;
	}

	void CTextureSet::AddFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
		m_FrameTextureList.push_back(FrameTexture);
	}

	const std::vector<std::shared_ptr<graphics::CTexture>>& CTextureSet::GetFrameTextureList() const
	{
		return m_FrameTextureList;
	}

	void CTextureSet::AddIBLTexture(const std::shared_ptr<graphics::CTexture>& IBL_Diffuse_Tex, const std::shared_ptr<graphics::CTexture>& IBL_Specular_Tex, const std::shared_ptr<graphics::CTexture>& IBL_GGXLUT_Tex)
	{
		m_IBL_Diffuse_Tex = IBL_Diffuse_Tex;
		m_IBL_Specular_Tex = IBL_Specular_Tex;
		m_BL_GGXLUT_Tex = IBL_GGXLUT_Tex;
	}

	const std::shared_ptr<graphics::CTexture>& CTextureSet::GetDiffuse_Tex() const
	{
		return m_IBL_Diffuse_Tex;
	}

	const std::shared_ptr<graphics::CTexture>& CTextureSet::GetSpecular_Tex() const
	{
		return m_IBL_Specular_Tex;
	}

	const std::shared_ptr<graphics::CTexture>& CTextureSet::GetGGXLUT_Tex() const
	{
		return m_BL_GGXLUT_Tex;
	}
}