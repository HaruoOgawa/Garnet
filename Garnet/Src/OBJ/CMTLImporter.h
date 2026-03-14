#pragma once

#include <vector>
#include <memory>
#include <string>

namespace api { class IGraphicsAPI; }
namespace object { class C3DObject; }
namespace graphics { class CMaterialFrame; }
namespace resource { class C3DObjectLoader; }

namespace obj
{
	class CMTLImporter
	{
		static void Analyze(const std::vector<std::string>& ParamList, const std::string& CurrentLine,
			const std::string& BaseDir, const std::shared_ptr<object::C3DObject>& Object, resource::C3DObjectLoader* p3DObjectLoader);

	public:
		static bool Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data,
			const std::string& BaseDir, const std::shared_ptr<object::C3DObject>& Object,
			const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList,
			resource::C3DObjectLoader* p3DObjectLoader);
	};
}