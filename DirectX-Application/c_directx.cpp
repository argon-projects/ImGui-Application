#include "c_directx.hpp"

HWND c_directx::hwnd;
LPCWSTR c_directx::window_name;
LPCWSTR c_directx::window_class;
ID3D11Device* c_directx::d3d_device;
ID3D11DeviceContext* c_directx::d3d_device_context;
IDXGISwapChain* c_directx::d3d_swapchain;
ID3D11RenderTargetView* c_directx::d3d_main_render_target_view;

c_directx::c_directx(LPCWSTR window_name, LPCWSTR window_class)
{
	c_directx::window_name = window_name;
	c_directx::window_class = window_class;

	c_directx::initialise();
}

void c_directx::initialise()
{
    // Create application window
  //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, c_directx::wnd_proc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, c_directx::window_class, NULL };
    ::RegisterClassEx(&wc);
    c_directx::hwnd = ::CreateWindow(wc.lpszClassName, c_directx::window_name, WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!c_directx::create_d3d_device())
    {
        c_directx::cleanup_d3d_device();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    }

    // Show the window
    //::ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();

    ImFontAtlas fontAtlas;
    auto defaultFont = c_ui_utils::load_font(fontAtlas, "segoeui.ttf", 22.0f);//16.0f * 96.0f / 72.0f);
    c_ui_utils::load_font(fontAtlas, "fa-solid-900.ttf", 22.0f);//16.0f * 96.0f / 72.0f);
    fontAtlas.Build();

    ImGuiContext& im_context = *ImGui::CreateContext(&fontAtlas);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();


    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(c_directx::d3d_device, c_directx::d3d_device_context);
    ImGui_ImplWin32_EnableDpiAwareness();
  

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
     
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_EnableDpiAwareness();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        c_ui::renderer();

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        c_directx::d3d_device_context->OMSetRenderTargets(1, &c_directx::d3d_main_render_target_view, NULL);
        c_directx::d3d_device_context->ClearRenderTargetView(c_directx::d3d_main_render_target_view, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        c_directx::d3d_swapchain->Present(1, 0); // Present with vsync
        //c_directx::d3d_swapchain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    c_directx::cleanup_d3d_device();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

}

bool c_directx::create_d3d_device()
{
    DXGI_SWAP_CHAIN_DESC swapchain_desc;
    ZeroMemory(&swapchain_desc, sizeof(swapchain_desc));
    swapchain_desc.BufferCount = 2;
    swapchain_desc.BufferDesc.Width = 0;
    swapchain_desc.BufferDesc.Height = 0;
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.BufferDesc.RefreshRate.Numerator = 60;
    swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.OutputWindow = c_directx::hwnd;
    swapchain_desc.SampleDesc.Count = 1;
    swapchain_desc.SampleDesc.Quality = 0;
    swapchain_desc.Windowed = TRUE;
    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT create_device_flags = 0;
    D3D_FEATURE_LEVEL feature_level;
    const D3D_FEATURE_LEVEL feature_level_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &swapchain_desc, &c_directx::d3d_swapchain, &c_directx::d3d_device, &feature_level, &c_directx::d3d_device_context) != S_OK)
        return false;

    c_directx::create_render_target();
    return true;
}

void c_directx::cleanup_d3d_device()
{
    c_directx::cleanup_render_target();
    if (c_directx::d3d_swapchain) { c_directx::d3d_swapchain->Release(); c_directx::d3d_swapchain = NULL; }
    if (c_directx::d3d_device_context) { c_directx::d3d_device_context->Release(); c_directx::d3d_device_context = NULL; }
    if (c_directx::d3d_device) { c_directx::d3d_device->Release(); c_directx::d3d_device = NULL; }
}

void c_directx::create_render_target()
{
    ID3D11Texture2D* pBackBuffer;
    c_directx::d3d_swapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    c_directx::d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &c_directx::d3d_main_render_target_view);
    pBackBuffer->Release();
}

void c_directx::cleanup_render_target()
{
    if (c_directx::d3d_main_render_target_view) { c_directx::d3d_main_render_target_view->Release(); c_directx::d3d_main_render_target_view = NULL; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI c_directx::wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (c_directx::d3d_device != NULL && wParam != SIZE_MINIMIZED)
        {
            c_directx::cleanup_render_target();
            c_directx::d3d_swapchain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            c_directx::create_render_target();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
