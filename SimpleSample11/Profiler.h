#pragma once

#include <dxgi.h>
#include <winerror.h>
#include <profileapi.h>
#include <winnt.h>

#include <utility>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace Util
{
	typedef long long qpc_t;
	const qpc_t kQpcMin = LONGLONG_MIN;

	inline double getQpcRate()
	{
		static double _CPUFrequency = -1.0;

		if ( _CPUFrequency == -1.0 ) {
			__int64 curr_rate = 0;
			::QueryPerformanceFrequency( (LARGE_INTEGER *)&curr_rate );
			_CPUFrequency = (double)curr_rate;
		}

		return _CPUFrequency;
	}


	inline double elapsedSeconds(qpc_t t0, qpc_t t1)
	{
		return static_cast<double>(t1 - t0)/ getQpcRate();
	}

	inline qpc_t qpcNow()
	{
		LARGE_INTEGER time;
		QueryPerformanceCounter( &time );
		return time.QuadPart;
	}

	inline std::string nowDateAndTimeAsString()
	{
		const auto now = std::chrono::system_clock::now();
		const auto now_c = std::chrono::system_clock::to_time_t(now);
		const auto now_tm = *std::localtime(&now_c);
		
		std::ostringstream oss;
		oss << std::put_time( &now_tm, "%H-%M-%S");
		return oss.str();
	}


	class LoopStats
	{
	public:
		LoopStats(unsigned index = 0) : index(index) {}

		unsigned index = 0;
		DXGI_FRAME_STATISTICS dxgi;
		HRESULT hrDxgi = S_FALSE;

		qpc_t qpcLoopBeg   = kQpcMin;
		qpc_t qpcLoopEnd   = kQpcMin;
		qpc_t qpcRenderBeg = kQpcMin;
		qpc_t qpcPresent   = kQpcMin;
	};

	std::pair<HRESULT, DXGI_FRAME_STATISTICS> getFrameStatistics(IDXGISwapChain& swap_chain);

	class Profiler
	{
	public:
		void onInitEnd()
		{
			m_initQpc = qpcNow();
			m_frameIndex = 1;

			m_curLoopStats = LoopStats{m_frameIndex};
			m_curLoopStats.qpcLoopBeg = m_initQpc;
		}

		void onRenderBegin() { m_curLoopStats.qpcRenderBeg = qpcNow(); }

		void onPresent()
		{
			assert(m_curLoopStats.qpcPresent == kQpcMin);
			m_curLoopStats.qpcPresent   = qpcNow();
		}

		void onPresentWndProc()
		{
			assert(false);
		}

		void record();

		void onNewLoop()
		{
			// Register current loop stats
			m_curLoopStats.qpcLoopEnd = qpcNow();
			m_allStats.push_back( m_curLoopStats );

			// Reinitialize stats for new loop
			++m_frameIndex;
			
			m_curLoopStats = LoopStats{m_frameIndex};
			m_curLoopStats.qpcLoopBeg = qpcNow();
		}

		static Profiler& get();

		Profiler() {}

	private:
		qpc_t m_initQpc = 0;
		unsigned m_frameIndex = 0;
		LoopStats m_curLoopStats;

		std::vector<LoopStats> m_allStats;
	};

}

