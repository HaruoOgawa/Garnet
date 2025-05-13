#include "CPresetPrimitive.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include <glm/glm.hpp>
#include "../Interface/IGraphicsAPI.h"

namespace graphics
{
	std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CPresetPrimitive::CreateFromType(api::IGraphicsAPI* pGraphicsAPI, graphics::EPresetPrimitiveType PresetType)
	{
		switch (PresetType)
		{
		case graphics::EPresetPrimitiveType::None:
			return std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());
		case graphics::EPresetPrimitiveType::BOARD:
			return CreateBoard(pGraphicsAPI);
		case graphics::EPresetPrimitiveType::CUBE:
			return CreateBox(pGraphicsAPI);
		case graphics::EPresetPrimitiveType::POINT:
			return CreatePoint(pGraphicsAPI);
		case graphics::EPresetPrimitiveType::SPHERE:
			return CreateSphere(pGraphicsAPI);
		case graphics::EPresetPrimitiveType::Max:
		default:
			return std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());
		}
	}

	std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CPresetPrimitive::CreateBoard(api::IGraphicsAPI* pGraphicsAPI)
	{
		std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> createInfo = std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());

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
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};

		std::vector<float> Tangent(16, 0.0f);
		std::vector<float> Joints(16, 0.0f);
		std::vector<float> Weights(16, 0.0f);

		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, Joints, Weights
		};

		// Indices
		std::vector<unsigned short> Indices = {
			0, 1, 2, 2, 3, 0,
		};

		//
		createInfo.first->SetVertices(Vertices);
		createInfo.second->SetIndices(Indices);
		createInfo.first->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4 }));
		createInfo.first->SetAttribDataTypes(std::vector<graphics::EDataType>({ graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT, graphics::EDataType::TYPE_UNSIGNED_INT, graphics::EDataType::TYPE_FLOAT }));
		createInfo.first->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}

	std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CPresetPrimitive::CreateBox(api::IGraphicsAPI* pGraphicsAPI)
	{
		std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> createInfo = std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());

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
		std::vector<float> Joints(96, 0.0f);
		std::vector<float> Weights(96, 0.0f);

		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, Joints, Weights
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
		createInfo.first->SetVertices(Vertices);
		createInfo.second->SetIndices(Indices);
		createInfo.first->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4 }));
		createInfo.first->SetAttribDataTypes(std::vector<graphics::EDataType>({ graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT, graphics::EDataType::TYPE_UNSIGNED_INT, graphics::EDataType::TYPE_FLOAT }));
		createInfo.first->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}

	std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CPresetPrimitive::CreatePoint(api::IGraphicsAPI* pGraphicsAPI)
	{
		std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> createInfo = std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());

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
		std::vector<float> Joints(4, 0.0f);
		std::vector<float> Weights(4, 0.0f);

		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, Joints, Weights
		};

		// Indices
		std::vector<unsigned short> Indices = {
			0
		};

		//
		createInfo.first->SetVertices(Vertices);
		createInfo.second->SetIndices(Indices);
		createInfo.first->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4 }));
		createInfo.first->SetAttribDataTypes(std::vector<graphics::EDataType>({ graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT, graphics::EDataType::TYPE_UNSIGNED_INT, graphics::EDataType::TYPE_FLOAT }));
		createInfo.first->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}
	
	std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CPresetPrimitive::CreateSphere(api::IGraphicsAPI* pGraphicsAPI)
	{
		std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> createInfo = std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());

		// Vertex Buffer
		std::vector<float> Pos;
		std::vector<float> Normal;
		std::vector<float> UV;
		std::vector<float> Tangent;
		std::vector<float> Joints;
		std::vector<float> Weights;

		// Indices
		std::vector<unsigned short> Indices;

		// Sphere Function
		// https://www.songho.ca/opengl/gl_sphere.html

		//
		float radius = 1.0f;
		float PI = 3.1415f;
		float sectorCount = 32.0f; // â°ÇÃêî
		float stackCount = 32.0f; // ècÇÃêî

		//
		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
		float s, t;                                     // vertex texCoord

		float sectorStep = 2 * PI / sectorCount;
		float stackStep = PI / stackCount;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= stackCount; ++i)
		{
			stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xy = radius * cosf(stackAngle);             // r * cos(u)
			z = radius * sinf(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= sectorCount; ++j)
			{
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// vertex position (x, y, z)
				x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
				Pos.push_back(x);
				Pos.push_back(y);
				Pos.push_back(z);

				// normalized vertex normal (nx, ny, nz)
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				Normal.push_back(nx);
				Normal.push_back(ny);
				Normal.push_back(nz);

				// vertex tex coord (s, t) range between [0, 1]
				s = (float)j / sectorCount;
				t = (float)i / stackCount;
				UV.push_back(s);
				UV.push_back(t);
			}
		}

		Tangent.resize(Pos.size() / 3 * 4, 0.0f);
		Joints.resize(Pos.size() / 3 * 4, 0.0f);
		Weights.resize(Pos.size() / 3 * 4, 0.0f);

		// generate CCW index list of sphere triangles
// k1--k1+1
// |  / |
// | /  |
// k2--k2+1
		int k1, k2;
		for (int i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);     // beginning of current stack
			k2 = k1 + sectorCount + 1;      // beginning of next stack

			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					Indices.push_back(k1);
					Indices.push_back(k2);
					Indices.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (stackCount - 1))
				{
					Indices.push_back(k1 + 1);
					Indices.push_back(k2);
					Indices.push_back(k2 + 1);
				}
			}
		}

		//
		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, Joints, Weights
		};

		createInfo.first->SetVertices(Vertices);
		createInfo.second->SetIndices(Indices);
		createInfo.first->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4}));
		createInfo.first->SetAttribDataTypes(std::vector<graphics::EDataType>({ graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT, graphics::EDataType::TYPE_UNSIGNED_INT, graphics::EDataType::TYPE_FLOAT }));
		createInfo.first->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}
}