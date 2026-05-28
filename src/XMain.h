#include <thread>
#include <mutex>
#include <chrono>
#include <unordered_map>

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

	injector::WriteMemory<int>(0x536386, 0x9D0, true);
	injector::WriteMemory<int>(0x537BB0, 0x9D0, true);

	WriteString<uint32_t>(0x6C94D6, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6C94E2, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6D6956, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6D6A7E, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6DDD1F, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6DDD34, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6DE653, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6DE739, gui_garage2.c_str(), true);

	page_name.reserve(128);
	numplate_filename.reserve(128);

	while (g_running.load(std::memory_order_relaxed))
	{
		XNumPlate();
		XWheels();
		XDynParts();
		XCarinfo();
		XEntriesList();
	}
}