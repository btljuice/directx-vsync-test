#pragma once

#include <dxgi.h>
#include <winerror.h>

#include <utility>

namespace Stats
{
	struct FrameStatistics
	{
		unsigned  presentCount = 0;
		unsigned  presentRefreshCount = 0;
		unsigned  syncRefreshCount = 0;
		long long syncQPCTime = 0;
		HRESULT   hr = 0;
	};

	FrameStatistics getFrameStatistics(IDXGISwapChain& swap_chain);
}

