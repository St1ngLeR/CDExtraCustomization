#include "injector/injector.hpp"

#include "cdmain.h"

#include "XGlobal.h"

#include "XCustom/XNumPlate.h"
#include "XCustom/XWheels.h"
#include "XCustom/XDynParts.h"

void Init()
{
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);

	while (true)
	{
		XNumPlate();
		XWheels();
		XDynParts();
	}
}