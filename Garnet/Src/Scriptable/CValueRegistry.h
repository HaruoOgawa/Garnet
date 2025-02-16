#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>

#include "../Graphics/EUniformValueType.h"
#include "../Timeline/CTimelineTrackContent.h"

namespace scene { class CSceneController; }

namespace scriptable
{
	struct SValue
	{
		std::string Name = std::string();
		graphics::EUniformValueType Type = graphics::EUniformValueType::NONE;
		int ByteSize = 0;
		std::vector<unsigned char> Buffer;
	};

	class CValueRegistry : public std::enable_shared_from_this<CValueRegistry>, public timeline::CTimelineTrackContent
	{
		const std::string m_RegistryName;

		std::map<std::string, SValue> m_ValueList;
		
	public:
		explicit CValueRegistry(const std::string& RegistryName);
		virtual ~CValueRegistry();

		virtual void OnLoaded(const std::shared_ptr<scene::CSceneController>& SceneController);

		const std::string& GetRegistryName() const;

		const std::map<std::string, SValue>& GetValueList() const;

		void SetValue(const std::string& Key, graphics::EUniformValueType ValueType, const void* Data, int ByteSize);
		void SetValue(const std::string& Key, const void* Data);
		SValue GetValue(const std::string& Key) const;
		float GetValueFloat(const std::string& Key) const;
		int GetValueInt(const std::string& Key) const;
		std::string GetValueString(const std::string& Key) const;
	};
}