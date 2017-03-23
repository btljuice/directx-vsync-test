#include "DXUT.h"

#include "FrameStatistics.h"

using namespace Stats;

FrameStatistics getFrameStatistics( IDXGISwapChain& swap_chain )
{
	FrameStatistics stats;

	DXGI_FRAME_STATISTICS dxgi_stats;
	ZeroMemory( &dxgi_stats, sizeof( dxgi_stats ) );
	stats.hr = swap_chain.GetFrameStatistics( &dxgi_stats );
	if ( stats.hr == S_OK )
	{
		stats.presentCount = dxgi_stats.PresentCount;
		stats.presentRefreshCount = dxgi_stats.PresentRefreshCount;
		stats.syncRefreshCount = dxgi_stats.SyncRefreshCount;
		stats.syncQPCTime = dxgi_stats.SyncQPCTime.QuadPart;
	}

	return stats;
}
