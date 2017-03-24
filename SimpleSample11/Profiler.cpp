#include "DXUT.h"

#include "Profiler.h"

#include <fstream>

using namespace Util;
using namespace std;

namespace
{
	Profiler g_singleton;
}


pair<HRESULT, DXGI_FRAME_STATISTICS> getFrameStatistics( IDXGISwapChain& swap_chain )
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

	o << "index, loopBeg, loopEnd, renderBeg, present" << std::endl;
	for ( const auto& s : m_allStats )
	{
		o << s.index
		  << ',' << elapsedSeconds(m_initQpc, s.qpcLoopBeg)
		  << ',' << elapsedSeconds(m_initQpc, s.qpcLoopEnd)
		  << ',' << elapsedSeconds(m_initQpc, s.qpcRenderBeg)
		  << ',' << elapsedSeconds(m_initQpc, s.qpcPresent)
		  << std::endl;
	}
}
