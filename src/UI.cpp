#include "UI.h"

void __stdcall SRTMGR::UI::DrawMainUI(bool &open)
{
	// If the Main UI is hidden, exit here.
	if (!open)
		return;

	// Conditionally shown items (shown only if the Main UI is showing)
	if (show_Debug_ImGuiDemo)
		ImGui::ShowDemoWindow(&show_Debug_ImGuiDemo);

	if (show_Help_About)
		DrawHelpAboutUI(show_Help_About);

	// Specify a default position/size in case there's no data in the .ini file.
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin(SRTMGR_APP_NAME, (bool *)&open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
		return;
	}

	// Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit", NULL, false, true))
				open = !open;
			ImGui::EndMenu();
		}

		if (IsDebug)
		{
			if (ImGui::BeginMenu("Debug"))
			{
				ImGui::MenuItem("ImGui Demo", NULL, &show_Debug_ImGuiDemo);
				ImGui::EndMenu();
			}
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About", NULL, &show_Help_About);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (IsDebug)
		SRTMGR::ImGuiExt::DrawFPSText();
	ImGui::End();
}

void __stdcall SRTMGR::UI::DrawHelpAboutUI(const bool &open)
{
	// Specify a default position/size in case there's no data in the .ini file.
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("About", (bool *)&open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	ImGui::Text(SRTMGR_APP_NAME);
	ImGui::Text("v%d.%d.%d (Build #%d)", SRTMGR_VERSION_MAJOR, SRTMGR_VERSION_MINOR, SRTMGR_VERSION_PATCH, SRTMGR_VERSION_BUILD);
	ImGui::Separator();
	ImGui::BulletText("Contributors\n\tSquirrelies");
	ImGui::Spacing();
	ImGui::Spacing();
	bool copyToClipboard = ImGui::Button("Copy to clipboard");
	ImGui::Spacing();
	if (ImGui::BeginChild("buildInfo", ImVec2(0, 0), ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (copyToClipboard)
		{
			ImGui::LogToClipboard();
			ImGui::LogText("```\n"); // Back quotes will make text appears without formatting when pasting on GitHub
		}

		ImGui::Text(SRTMGR_APP_NAME);
		ImGui::Text("v%d.%d.%d (Build #%d)", SRTMGR_VERSION_MAJOR, SRTMGR_VERSION_MINOR, SRTMGR_VERSION_PATCH, SRTMGR_VERSION_BUILD);
		ImGui::Separator();
		ImGui::Text("Build datetime: %s %s", __DATE__, __TIME__);
		ImGui::Text("Debug build: %s", IsDebug ? "true" : "false");
		ImGui::Text("sizeof(void *): %d", (int)sizeof(void *));
#ifdef _WIN32
		ImGui::Text("define: _WIN32");
#endif
#ifdef _WIN64
		ImGui::Text("define: _WIN64");
#endif
		ImGui::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef __STDC__
		ImGui::Text("define: __STDC__=%d", (int)__STDC__);
#endif
#ifdef __STDC_VERSION__
		ImGui::Text("define: __STDC_VERSION__=%d", (int)__STDC_VERSION__);
#endif
#ifdef __GNUC__
		ImGui::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
		ImGui::Text("define: __clang_version__=%s", __clang_version__);
#endif

#ifdef _MSC_VER
		ImGui::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef _MSVC_LANG
		ImGui::Text("define: _MSVC_LANG=%d", (int)_MSVC_LANG);
#endif
#ifdef __MINGW32__
		ImGui::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
		ImGui::Text("define: __MINGW64__");
#endif

		if (copyToClipboard)
		{
			ImGui::LogText("\n```");
			ImGui::LogFinish();
		}
		ImGui::EndChild();
	}

	ImGui::End();
}

SRTMGR::UI::UI()
{
	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return;
	}

	// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	// Setup window
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_HIDDEN);
	window = SDL_CreateWindow(SRTMGR_APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1, 1, window_flags);
	if (window == nullptr)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return;
	}

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hwnd = (HWND)wmInfo.info.win.window;

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		return;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.IniFilename = "SRTManager.ini";
	io.LogFilename = "SRTManager.log";
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
	// io.ConfigViewportsNoAutoMerge = true;
	// io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle &style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForD3D(window);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	// io.Fonts->AddFontDefault();
	// io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	// ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	// IM_ASSERT(font != nullptr);

	// Our state
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

SRTMGR::UI::~UI()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void SRTMGR::UI::RenderLoop(void)
{
	while (show_Main)
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				show_Main = false;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				show_Main = false;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(window))
			{
				// Release all outstanding references to the swap chain's buffers before resizing.
				CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		DrawMainUI(show_Main);

		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w};
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		ImGuiIO &io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		// g_pSwapChain->Present(1, 0); // Present with vsync
		g_pSwapChain->Present(0, 0); // Present without vsync
	}
}

bool SRTMGR::UI::CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	// createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = {
	    D3D_FEATURE_LEVEL_11_0,
	    D3D_FEATURE_LEVEL_10_0,
	};
	if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void SRTMGR::UI::CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain)
	{
		g_pSwapChain->Release();
		g_pSwapChain = nullptr;
	}
	if (g_pd3dDeviceContext)
	{
		g_pd3dDeviceContext->Release();
		g_pd3dDeviceContext = nullptr;
	}
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = nullptr;
	}
}

void SRTMGR::UI::CreateRenderTarget()
{
	ID3D11Texture2D *pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void SRTMGR::UI::CleanupRenderTarget()
{
	if (g_mainRenderTargetView)
	{
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = nullptr;
	}
}
