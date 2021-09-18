#pragma once
#include "pch.hpp"
class c_directx
{
public:
	c_directx(LPCWSTR window_name, LPCWSTR window_class);
	static HWND hwnd;
private:

	static LPCWSTR window_name;
	static LPCWSTR window_class;
	static ID3D11Device* d3d_device;
	static ID3D11DeviceContext* d3d_device_context;
	static IDXGISwapChain* d3d_swapchain;
	static ID3D11RenderTargetView* d3d_main_render_target_view;

	static void initialise();
	static bool create_d3d_device();
	static void cleanup_d3d_device();
	static void create_render_target();
	static void cleanup_render_target();

	static LRESULT WINAPI wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

