#pragma once
namespace DM
{
	class DM_API GraphicsContext
	{
	public:
		virtual void Init()=0;
		virtual void SwapBuffers()=0;
	};
}
