#include "CGraphicsEditingWindow.h"

#ifdef USE_GUIENGINE
namespace gui
{
	CGraphicsEditingWindow::CGraphicsEditingWindow():
		m_TestFloatVal(0.0f),
		m_TestFloatSliderVal(0.0f)
	{
	}

	CGraphicsEditingWindow::~CGraphicsEditingWindow()
	{
	}

	bool CGraphicsEditingWindow::Draw()
	{
		// Start Example ///////////////////////////////////////////////////////////////////////////////
		ImGui::Begin("Hello World");

		ImGui::Text("This is Hello World Text");

		static int counter = 0;

		ImGui::SliderFloat("float", &m_TestFloatSliderVal, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		//
		ImGui::BeginTabBar("TabBar1");
		static bool Flag = true;
		//ImGui::TabItemButton("TabTest", Flag);

		ImGui::BeginTabItem("TabItemTest", &Flag);
		ImGui::InputFloat("TestInput", &m_TestFloatVal);
		ImGui::EndTabItem();

		ImGui::EndTabBar();

		ImGui::End();

		// End Example ///////////////////////////////////////////////////////////////////////////////

		return true;
	}
}
#endif // USE_GUIENGINE