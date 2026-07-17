#pragma once
#include <chrono>
#include <Core/Core.h>

namespace DM
{
	// 时钟系统（L2 Core 层基础服务）。
	// 提供：可变帧间隔、固定步长、时间缩放、高精度时间戳。
	// 物理系统等需要固定步长循环可调用 ShouldStepFixed()。
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

		// 每帧开始调用，返回本帧的可变 delta time（已乘时间缩放）。
		float Tick()
		{
			auto now = std::chrono::steady_clock::now();
			std::chrono::duration<float> diff = now - m_Last;
			m_Last = now;
			float dt = diff.count() * m_TimeScale;
			m_Accumulator += dt;
			return dt;
		}

		// 固定步长循环：物理系统等在返回 true 时执行一次固定步长更新。
		bool ShouldStepFixed()
		{
			if (m_Accumulator >= m_FixedDt)
			{
				m_Accumulator -= m_FixedDt;
				return true;
			}
			return false;
		}

		// 取一次固定步长（供调用方使用）。
		float FixedDt() const { return m_FixedDt; }

		// 设置时间缩放（0 暂停，>1 加速）。
		void SetTimeScale(float scale) { m_TimeScale = scale; }
		float GetTimeScale() const { return m_TimeScale; }

		// 高精度时间戳（秒）。
		static double Now()
		{
			auto now = std::chrono::steady_clock::now();
			return std::chrono::duration<double>(now.time_since_epoch()).count();
		}

	private:
		float m_FixedDt;
		float m_Accumulator;
		float m_TimeScale;
		std::chrono::steady_clock::time_point m_Last;
	};
}
