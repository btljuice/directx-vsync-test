#include "DXUT.h"

#include "Profiler.h"

#include <fstream>
#include <tuple>

using namespace Util;
using namespace std;

namespace
{
	Profiler g_singleton;
}


pair<HRESULT, DXGI_FRAME_STATISTICS> Util::getFrameStatistics( IDXGISwapChain& swap_chain )
{
	DXGI_FRAME_STATISTICS s;
	HRESULT hr;
	ZeroMemory( &s, sizeof( s ) );
	hr = swap_chain.GetFrameStatistics( &s );

	return { hr, s };
}

Profiler& Profiler::get() { return g_singleton; }

void Profiler::record()
{
	const auto filename = string{"profiler_"} +nowDateAndTimeAsString() + ".csv";

	std::ofstream o( filename);

	o << "index, loopBeg, loopEnd, renderBeg, present, dxgi.hresult, dxgi.PresentCount, dxgi.PresentRefreshCount, dxgi.SyncRefreshCount, dxgi.SyncQPCTime" << std::endl;
    LARGE_INTEGER SyncGPUTime;
	for ( const auto& s : m_allStats )
	{
		o << s.index
		  << ',' << elapsedSeconds(m_initQpc, s.qpcLoopBeg)
		  << ',' << elapsedSeconds(m_initQpc, s.qpcLoopEnd)
		  << ',' << elapsedSeconds(m_initQpc, s.qpcRenderBeg)
		  << ',' << elapsedSeconds(m_initQpc, s.qpcPresent)
		  << ',' << s.hrDxgi
		  << ',' << s.dxgi.PresentCount
		  << ',' << s.dxgi.PresentRefreshCount
		  << ',' << s.dxgi.SyncRefreshCount
		  << ',' << elapsedSeconds(m_initQpc, s.dxgi.SyncQPCTime.QuadPart)
		  << std::endl;
	}
}

void Util::Profiler::onPresent(IDXGISwapChain& swap_chain)
{
	assert( m_curLoopStats.qpcPresent == kQpcMin );
	m_curLoopStats.qpcPresent = qpcNow();

	if (DXUTIsFullScreen())
	{
		std::tie(m_curLoopStats.hrDxgi, m_curLoopStats.dxgi) = getFrameStatistics(swap_chain);
	}
	else
	{
		m_curLoopStats.hrDxgi = S_FALSE;
	}
}


