#include "CPresetPrimitive.h"
#include "../GraphicsAPI/CRendererCreateInfo.h"

namespace graphics
{
	bool CPresetPrimitive::CreateBoard(renderer::CRendererCreateInfo& createInfo)
	{
		// Vertex Buffer
		std::vector<float> Pos = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f
		};

		std::vector<float> Color = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f
		};

		std::vector<float> UV = {
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};

		std::vector<std::vector<float>> Vertices = {
			Pos, Color, UV
		};

		// Indices
		std::vector<uint16_t> Indices = {
			0, 1, 2, 2, 3, 0,
		};

		//
		createInfo.SetVertices(Vertices);
		createInfo.SetIndices(Indices);
		createInfo.SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2 }));

		return true;
	}
}