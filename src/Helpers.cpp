#include "Helpers.h"

void SRTMGR::DrawFPSText(void)
{
	static const float halfCharSize = ImGui::CalcTextSize(" ").x / 2.f;
	ImGuiIO &io = ImGui::GetIO();
	const char fpsFormat[] = "Application average %.3f ms/frame (%.1f FPS)";

	std::unique_ptr<std::string> fpsFormattedText = GetPrintfFormattedString(fpsFormat, 1000.0f / io.Framerate, io.Framerate);
	ImVec2 fpsTextSize = ImGui::CalcTextSize(fpsFormattedText.get()->c_str());

	ImGui::SameLine(ImGui::GetWindowWidth() - (fpsTextSize.x + halfCharSize));
	ImGui::Text("%s", fpsFormattedText.get()->c_str());
}

std::unique_ptr<std::string> SRTMGR::GetPrintfFormattedString(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	int size = vsnprintf(nullptr, 0, fmt, args) + 1;

	std::unique_ptr<std::string> returnValue = std::make_unique<std::string>();
	returnValue.get()->reserve(size);
	vsnprintf(returnValue.get()->data(), size, fmt, args);

	va_end(args);

	return returnValue;
}
