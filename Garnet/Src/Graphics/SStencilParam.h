#pragma once

namespace graphics
{
    enum class EStencilFunc
    {
        Never,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,
        Equal,
        NotEqual,
        Always,
    };

    enum class EStencilOp // Op : Operation
    {
        Keep,
        Replace,
        Incr,
        Decr,
    };

	struct SStencilParam
	{
		bool Enabled = false;
        EStencilFunc Func = EStencilFunc::Always;
        char RefValue = 0;
        unsigned char Mask = 0xff;
        EStencilOp SFail = EStencilOp::Keep;  // action to take if the stencil test fails.
        EStencilOp DpFail = EStencilOp::Keep; // action to take if the stencil test passes, but the depth test fails.
        EStencilOp DpPass = EStencilOp::Keep; // action to take if both the stencil and the depth test pass.
	};
}