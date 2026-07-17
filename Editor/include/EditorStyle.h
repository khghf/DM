#pragma once
namespace DM
{
	class EditorStyle
	{
	public:
		static void SetDefaultStyle();
	private:
		static void SetupImGuiDarkLuxuryTheme(float alpha = 1.0f);
	};
}


