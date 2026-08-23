#pragma once
#include <chrono>
#include <Core/Core.h>

namespace DM
{
	class DM_API Clock
	{
	public:
		Clock(float fixedDt = 1.f / 60.f)
			: m_FixedDt(fixedDt)
			, m_Accumulator(0.f)
			, m_TimeScale(1.f)
			, m_Last(std::chrono::steady_clock::now())
		{
		}
		
		float Tick()
		{
			auto now = std::chrono::steady_clock::now();
			std::chrono::duration<float> diff = now - m_Last;
			m_Last = now;
			float dt = diff.count() * m_TimeScale;
			m_Accumulator += dt;
			m_FramRate = dt / 1.f;
			return dt;
		}

		bool ShouldStepFixed()
		{
			if (m_Accumulator >= m_FixedDt)
			{
				m_Accumulator -= m_FixedDt;
				return true;
			}
			return false;
		}

		float		FixedDt() const { return m_FixedDt; }

		void		SetTimeScale(float scale) { m_TimeScale = scale; }
		float		GetTimeScale() const	{ return m_TimeScale; }
		uint32_t	GetFrameRate()const { return m_FramRate; }
		static double Now()
		{
			auto now = std::chrono::steady_clock::now();
			return std::chrono::duration<double>(now.time_since_epoch()).count();
		}

	private:
		float		m_FixedDt;
		float		m_Accumulator;
		float		m_TimeScale;
		uint32_t	m_FramRate;
		std::chrono::steady_clock::time_point m_Last;
	};
}
