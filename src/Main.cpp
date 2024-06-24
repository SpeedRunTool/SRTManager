#include "Main.h"

int main(int UNUSED(argc), char *UNUSED(argv[]))
{
	std::unique_ptr<SRTMGR::UI> ui = std::make_unique<SRTMGR::UI>();
	ui.get()->RenderLoop();

	return 0;
}
