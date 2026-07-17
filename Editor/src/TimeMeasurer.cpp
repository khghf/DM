//#include<DMPCH.h>
//#include"Tool/Util/TimeMeasurer.h"
//#include<imgui.h>
//namespace DM
//{
//	TimeMeasurerLayer* TimeMeasurerLayer::m_Inst = nullptr;
//	void TimeMeasurerLayer::Render()
//	{
//		ImGui::Begin("TimeMeasurer");
//		for (const auto& it : m_Times)
//		{
//			ImGui::SeparatorText(it.first.c_str());
//			for (const auto& res : it.second)
//			{
//				ImGui::Text("%.3fms	%s", res.Duration, res.FunName.data());
//			}
//		}
//		m_Times.clear();
//		ImGui::End();
//	}
//}