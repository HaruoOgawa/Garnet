#pragma once

// —e—Ê‚ª‚©‚³‚Ş‚Ì‚ÅÅ¬ŒÀ‚ÌÀ‘•‚Å
namespace object
{
	class CObject
	{
	public:
		CObject() = default;
		virtual ~CObject() = default;

		virtual bool Update(float Time);
		virtual bool Draw();
	};
}