#pragma once
#include "../../main.hpp"
#include "../menu/menu.hpp"

class c_overlay
{
public:
    static void start( );

private:
    static ID3D11Device* d3d_device;
    static ID3D11DeviceContext* d3d_device_context;
    static IDXGISwapChain* swap_chain;
    static UINT resize_width;
    static UINT resize_height;
    static ID3D11RenderTargetView* render_target_view;

    static ID3D11DepthStencilState* original_depth_stencil_state; // To store the original state
    static ID3D11DepthStencilState* no_depth_stencil_state;       // Custom state for drawing through walls

    static bool create_device_d3d( HWND hWnd );
    static void cleanup_device_d3d( );
    static void create_render_target( );
    static void cleanup_render_target( );
    static LRESULT WINAPI wnd_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
};

inline c_overlay overlay;