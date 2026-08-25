#pragma once
namespace DM
{
	class ImGuiRenderer;
	class ImGuiInitializer
	{
	public:
		static void Init(ImGuiRenderer*imGuiRenderer);

		static void ShutDown();

	};

}