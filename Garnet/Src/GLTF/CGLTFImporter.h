#pragma once

#include <vector>
#include <memory>
#include <string>

namespace object { class C3DObject; }

namespace gltf
{
	class CGLTFImporter
	{
	public:
		static bool Create(const std::vector<char>& Data, std::shared_ptr<object::C3DObject>& Object);
	};
}