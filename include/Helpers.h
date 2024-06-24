#ifndef SRTMGR_HELPERS_H
#define SRTMGR_HELPERS_H

#include "Common.h"
#include <imgui.h>
#include <memory>
#include <string>

namespace SRTMGR
{
	std::unique_ptr<std::string> GetPrintfFormattedString(const char *fmt, ...);
	void DrawFPSText(void);
}

#endif
