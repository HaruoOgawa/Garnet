#include "CPresetPrimitive.h"
#include "../GraphicsAPI/CRendererCreateInfo.h"

namespace graphics
{
	bool CPresetPrimitive::CreateBoard(std::shared_ptr<renderer::CRendererCreateInfo>& createInfo)
	{
		// Vertex Buffer
		std::vector<float> Pos = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f
		};

		std::vector<float> Normal = {
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
		};

		std::vector<float> UV = {
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};

		std::vector<float> Tangent(16, 0.0f);
		std::vector<float> BioTangent(16, 0.0f);

		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, BioTangent
		};

		// Indices
		std::vector<unsigned short> Indices = {
			0, 1, 2, 2, 3, 0,
		};

		//
		createInfo->SetVertices(Vertices);
		createInfo->SetIndices(Indices);
		createInfo->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4 }));

		return true;
	}

	bool CPresetPrimitive::CreateBox(std::shared_ptr<renderer::CRendererCreateInfo>& createInfo)
	{
		// Vertex Buffer
		std::vector<float> Pos = {
			//0 +Z
			-0.5f,-0.5f,0.5f, // 0
			0.5f,0.5f,0.5f,   // 1
			-0.5f,0.5f,0.5f,  // 2
			0.5f,-0.5f,0.5f,  // 3
			//1 -Z
			0.5f,-0.5f,-0.5f, // 4 // ó†(-) Ç…çsÇ≠Ç∆Ç´ÇÕç∂âEîΩì]
			-0.5f,0.5f,-0.5f, // 5
			0.5f,0.5f,-0.5f,  // 6
			-0.5f,-0.5f,-0.5f,// 7
			//2 -X
			-0.5f,0.5f,-0.5f,
			-0.5f,-0.5f,0.5f,
			-0.5f,0.5f,0.5f,
			-0.5f,-0.5f,-0.5f,
			//3 +X
			0.5f,-0.5f,-0.5f,
			0.5f,0.5f,0.5f,
			0.5f,-0.5f,0.5f,
			0.5f,0.5f,-0.5f,
			//4  -Y
			-0.5f,-0.5f,-0.5f,
			0.5f,-0.5f,0.5f,
			-0.5f,-0.5f,0.5f,
			0.5f,-0.5f,-0.5f,
			//5 +Y
			0.5f,0.5f,-0.5f,
			-0.5f,0.5f,0.5f,
			0.5f,0.5f,0.5f,
			-0.5f,0.5f,-0.5f,
		};

		std::vector<float> Normal = {
			//0 +Z
			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,
			//1 -Z
			0.0f,0.0f,-1.0f,
			0.0f,0.0f,-1.0f,
			0.0f,0.0f,-1.0f,
			0.0f,0.0f,-1.0f,
			//2 -X
			-1.0f,0.0f,0.0f,
			-1.0f,0.0f,0.0f,
			-1.0f,0.0f,0.0f,
			-1.0f,0.0f,0.0f,
			//3 +X
			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,
			//4 -Y
			0.0f,-1.0f,0.0f,
			0.0f,-1.0f,0.0f,
			0.0f,-1.0f,0.0f,
			0.0f,-1.0f,0.0f,
			//5 +Y
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,
		};

		std::vector<float> UV = {
			//0 +Z
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
			//1 -Z
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
			//2 -X
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
			//3 +X
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
			//4 -Y
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
			//5 +Y
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
		};

		std::vector<float> Tangent(96, 0.0f);
		std::vector<float> BioTangent(96, 0.0f);

		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, BioTangent
		};

		// Indices
		std::vector<unsigned short> Indices = {
			//0 +Z
			0,1,2,
			0,3,1,
			//1 -Z
			4,5,6,
			4,7,5,
			//2 -X
			8,9,10,
			8,11,9,
			//3 +X
			12,13,14,
			12,15,13,
			//4 -Y
			16,17,18,
			16,19,17,
			//5 +Y
			20,21,22,
			20,23,21,
		};

		//
		createInfo->SetVertices(Vertices);
		createInfo->SetIndices(Indices);
		createInfo->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4 }));

		return true;

		return true;
	}

	bool CPresetPrimitive::CreatePoint(std::shared_ptr<renderer::CRendererCreateInfo>& createInfo)
	{
		// Vertex Buffer
		std::vector<float> Pos = {
			0.0f, 0.0f, 0.0f,
		};

		std::vector<float> Normal = {
			0.0f, 0.0f, 0.0f,
		};

		std::vector<float> UV = {
			0.0f, 0.0f,
		};

		std::vector<float> Tangent(4, 0.0f);
		std::vector<float> BioTangent(4, 0.0f);

		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, BioTangent
		};

		// Indices
		std::vector<unsigned short> Indices = {
			0
		};

		//
		createInfo->SetVertices(Vertices);
		createInfo->SetIndices(Indices);
		createInfo->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4 }));

		return true;

		return true;
	}
}