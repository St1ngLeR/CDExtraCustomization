#include <thread>
#include <mutex>
#include <chrono>

#include "injector/injector.hpp"

#include "cdmain.h"

#include "XGlobal.h"

#include "XCustom/XNumPlate.h"
#include "XCustom/XWheels.h"
#include "XCustom/XDynParts.h"
#include "XCustom/XCarinfo.h"
#include "XCustom/XEntitiesList.h"

using namespace std::chrono_literals;

namespace
{
	std::atomic<bool> g_running{ true };
}

void Init()
{
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);

	while (g_running.load(std::memory_order_relaxed))
	{
		XNumPlate();
		XWheels();
		XDynParts();
		XCarinfo();
		XEntriesList();
	}
}