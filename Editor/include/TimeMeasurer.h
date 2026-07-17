//#pragma once
//#include<chrono>
//#include<Core/Layer/Layer.h>
//#include<Tool/ISingletion.h>
//namespace DM
//{
//	class TimeMeasurer
//	{
//	public:
//		struct Result
//		{
//			std::string ClassName;
//			std::string_view FunName;
//			double Duration;
//			Result(std::string className, std::string_view funName,float duration) :ClassName(className), FunName(funName),Duration(duration) {}
//		};
//		using StopCallback = void(*)(Result);
//	public:
//		TimeMeasurer(std::string className, std::string_view funName, StopCallback callback):m_ClassName(className),m_FunName(funName), m_CallbackFun(callback)
//		{
//			Start();
//		}
//		~TimeMeasurer()
//		{
//			if (bStop)return;
//			Stop();
//		}
//	private:
//		void Start()
//		{
//			m_StartTimePoint =std::chrono::high_resolution_clock::now();
//		}
//		void Stop()
//		{
//			bStop = true;
//			auto endTimepoint = std::chrono::high_resolution_clock::now();
//			auto duration=std::chrono::duration_cast<std::chrono::microseconds>(endTimepoint - m_StartTimePoint);
//			m_CallbackFun({ m_ClassName, m_FunName ,duration.count()*0.001f });
//		}
//	private:
//		bool bStop = false;
//		std::chrono::high_resolution_clock::time_point m_StartTimePoint;
//		std::string m_ClassName;
//		std::string_view m_FunName;
//		StopCallback m_CallbackFun;
//	};
//
//	class TimeMeasurerLayer:public Layer
//	{
//		friend class GEngine;
//	public:
//		static TimeMeasurerLayer* GetInst()
//		{
//			if (m_Inst == nullptr)
//			{
//				m_Inst = new TimeMeasurerLayer();
//			}
//			return m_Inst;
//		}
//		void Push(TimeMeasurer::Result time)
//		{
//			m_Times[time.ClassName].push_back(time);
//			m_ClassTime[time.ClassName] += (float)time.Duration;
//		}
//		virtual void OnEvent(Event* const e) { e->bHandled = true; }
//	private:
//		TimeMeasurerLayer() :Layer("TimeMeasurer") {}
//		virtual void Render()override;
//	private:
//		std::unordered_map<std::string, std::vector<TimeMeasurer::Result>>m_Times;
//		std::unordered_map<std::string, float>m_ClassTime;
//		static TimeMeasurerLayer* m_Inst;
//	};
//}
//#if 1
//#define PROFILE_SCOPE() \
//				{\
//					std::string filePath=__FILE__;\
//					size_t sepIndex = filePath.find_last_of("/\\"); \
//					std::string fileName = (sepIndex != std::string::npos) ? filePath.substr(sepIndex + 1) : filePath;\
//					size_t dotIndex = fileName.find_last_of('.');\
//					std::string className = (dotIndex != std::string::npos) ? fileName.substr(0, dotIndex) : fileName;\
//					if (className.empty()) {\
//						className = "UnknownClass";\
//					}\
//					DM::TimeMeasurer time##__LINE__(className,__FUNCTION__,\
//													[](DM::TimeMeasurer::Result time)\
//													{\
//														DM::TimeMeasurerLayer::GetInst()->Push(time);\
//													}); \
//				}
//#endif // 1
