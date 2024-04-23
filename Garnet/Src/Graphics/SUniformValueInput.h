#pragma once

namespace graphics
{
	enum class EUniformInputType
	{
		None = -1,
		Range,
		Color,
	};

	struct SUniformValueInput
	{
		EUniformInputType Type = EUniformInputType::None;
		float MinValue = 0.0f;
		float MaxValue = 0.0f;
		bool Hide = false;
	};
}