#pragma once
#ifdef USE_WEBGPU
#include "../../Graphics/CTexture.h"
#include "CWebGPUAPI.h"

namespace api
{
	class CWebGPUAPI;

	class CWebGPUTexture : public graphics::CTexture
	{
		// API
		api::CWebGPUAPI* m_pGraphicsAPI;

		// Texture
		WGPUTextureView m_TextureImageView;
		WGPUSampler		m_TextureSampler;
	private:
		bool CreateFrameTextureImageView(api::ERenderPassFormat RenderPassFormat);
		bool CreateTextureImageView(const std::vector<unsigned char>& OriginalPixels, int pixelSize);
		bool CreateTextureSampler();

		// Helper Function ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool ComputeShrinkPixels(const std::vector<unsigned char>& SrcPixels, std::vector<unsigned char>& DstPixles, uint32_t w, uint32_t h);
	public:
		CWebGPUTexture(api::CWebGPUAPI* pGraphicsAPI, bool UseMipMap, const graphics::STextureSamplerParam& SamplerParam);
		virtual ~CWebGPUTexture();

		virtual bool CreateFrameTexture(int Width, int Height, api::ERenderPassFormat RenderPassFormat, int AASampleNum, bool ReadOnShader) override;
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) override;

		const WGPUTextureView& GetTextureImageView() const;
		const WGPUSampler& GetTextureSampler() const;
	};
}
#endif