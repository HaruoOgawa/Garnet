#pragma once
#ifdef __DAWN__
#include <webgpu.h>
#include <wgpu.h>
#include "../../Graphics/CTexture.h"

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
		bool CreateTextureImageView(const std::vector<unsigned char>& OriginalPixels, int pixelSize);
		bool CreateTextureSampler();

		// Helper Function ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool ComputeShrinkPixels(const std::vector<unsigned char>& SrcPixels, std::vector<unsigned char>& DstPixles, uint32_t w, uint32_t h);
	public:
		CWebGPUTexture(api::CWebGPUAPI* pGraphicsAPI, bool UseMipMap);
		virtual ~CWebGPUTexture();

#ifdef USE_TEXTURE_LOADER
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) override;
#endif
		const WGPUTextureView& GetTextureImageView() const;
		const WGPUSampler& GetTextureSampler() const;
	};
}
#endif