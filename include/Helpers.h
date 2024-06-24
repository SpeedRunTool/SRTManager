#ifndef SRTMGR_HELPERS_H
#define SRTMGR_HELPERS_H

#include "Common.h"
#include <imgui.h>
#include <memory>
#include <string>

namespace SRTMGR
{
	std::unique_ptr<std::string> GetPrintfFormattedString(const char *, ...);

	namespace ImGuiExt
	{
		void RightJustify(std::string &);
		void DrawFPSText(void);
	}
}

#endif
