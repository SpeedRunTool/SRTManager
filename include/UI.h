#ifndef SRTMGR_UI_H
#define SRTMGR_UI_H

#include "Common.h"
#include "Helpers.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_sdl2.h>
#include <memory>
#include <string>

namespace SRTMGR
{
	class UI
	{
	private:
		bool show_Main = true;
		bool show_Debug_ImGuiDemo = false;
		bool show_Help_About = false;

		ID3D11Device *g_pd3dDevice = nullptr;
		ID3D11DeviceContext *g_pd3dDeviceContext = nullptr;
		IDXGISwapChain *g_pSwapChain = nullptr;
		ID3D11RenderTargetView *g_mainRenderTargetView = nullptr;
		SDL_Window *window;
		ImVec4 clear_color;

		bool CreateDeviceD3D(HWND hWnd);
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();

		void __stdcall DrawMainUI(bool &);
		void __stdcall DrawHelpAboutUI(const bool &);

	protected:
	public:
		UI();
		~UI();
		void RenderLoop(void);
	};
}

#endif
