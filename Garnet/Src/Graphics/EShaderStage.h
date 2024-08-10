#pragma once

namespace graphics
{
	enum class EShaderStage
	{
		SHADER_STAGE_NONE = -1,

		SHADER_STAGE_VERTEX,
		SHADER_STAGE_FRAGMENT,
		SHADER_STAGE_COMPUTE,
		SHADER_STAGE_GEOMETRY,
		SHADER_STAGE_HULL,
		SHADER_STAGE_DOMAIN,
	};
}
