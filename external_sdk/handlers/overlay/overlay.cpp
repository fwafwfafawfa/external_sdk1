#include "overlay.hpp"

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dwmapi.lib" )

#include "../../game/features/handler.hpp"
#include "../menu/menu.hpp"
#include "../themes/theme.hpp"
#include "../workspaceviewer/workspaceviewer.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

ID3D11Device * c_overlay::d3d_device = nullptr;
ID3D11DeviceContext * c_overlay::d3d_device_context = nullptr;
IDXGISwapChain * c_overlay::swap_chain = nullptr;
UINT c_overlay::resize_width = 0;
UINT c_overlay::resize_height = 0;
ID3D11RenderTargetView * c_overlay::render_target_view = nullptr;

ID3D11DepthStencilState * c_overlay::original_depth_stencil_state = nullptr;
ID3D11DepthStencilState * c_overlay::no_depth_stencil_state = nullptr;

bool c_overlay::create_device_d3d( HWND hWnd )
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 240;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT create_device_flags = 0;
    D3D_FEATURE_LEVEL feature_level;
    const D3D_FEATURE_LEVEL feature_level_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    HRESULT res = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags,
                                                  feature_level_array, 2, D3D11_SDK_VERSION, &sd, &swap_chain,
                                                  &d3d_device, &feature_level, &d3d_device_context );
    if ( res == DXGI_ERROR_UNSUPPORTED )
    {
        res = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags,
                                             feature_level_array, 2, D3D11_SDK_VERSION, &sd, &swap_chain,
                                             &d3d_device, &feature_level, &d3d_device_context );
    }
    if ( res != S_OK )
    {
        return false;
    }

    create_render_target( );

    // Create Depth Stencil States
    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
    d3d_device->CreateDepthStencilState(&depth_stencil_desc, &original_depth_stencil_state); // Default state

    depth_stencil_desc.DepthEnable = FALSE; // Disable depth testing
    d3d_device->CreateDepthStencilState(&depth_stencil_desc, &no_depth_stencil_state);

    return true;
}

void c_overlay::cleanup_device_d3d( )
{
    cleanup_render_target( );
    if ( swap_chain )
    {
        swap_chain->Release( );
        swap_chain = nullptr;
    }
    if ( d3d_device_context )
    {
        d3d_device_context->Release( );
        d3d_device_context = nullptr;
    }
    if ( d3d_device )
    {
        if (original_depth_stencil_state) {
            original_depth_stencil_state->Release();
            original_depth_stencil_state = nullptr;
        }
        if (no_depth_stencil_state) {
            no_depth_stencil_state->Release();
            no_depth_stencil_state = nullptr;
        }

        d3d_device->Release( );
        d3d_device = nullptr;
    }
}

void c_overlay::create_render_target( )
{
    ID3D11Texture2D * p_back_buffer = nullptr;
    swap_chain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< LPVOID * >( &p_back_buffer ) );
    d3d_device->CreateRenderTargetView( p_back_buffer, nullptr, &render_target_view );
    if ( p_back_buffer )
    {
        p_back_buffer->Release( );
    }
}

void c_overlay::cleanup_render_target( )
{
    if ( render_target_view )
    {
        render_target_view->Release();
        render_target_view = nullptr;
    }
}

LRESULT WINAPI c_overlay::wnd_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
    {
        return true;
    }

    switch ( msg )
    {
    case WM_SIZE:
        if ( wParam == SIZE_MINIMIZED )
        {
            return 0;
        }
        resize_width = static_cast< UINT >( LOWORD( lParam ) );
        resize_height = static_cast< UINT >( HIWORD( lParam ) );
        return 0;
    case WM_SYSCOMMAND:
        if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
        {
            return 0;
        }
        break;
    case WM_DESTROY:
        ::PostQuitMessage( 0 );
        return 0;
    }

    return ::DefWindowProcW( hWnd, msg, wParam, lParam );
}

#include "../../game/features/noclip/noclip.hpp"

void c_overlay::start( )
{
    WNDCLASSEXW wc = { sizeof( wc ), CS_CLASSDC, wnd_proc, 0L, 0L, GetModuleHandle( nullptr ), nullptr, nullptr, nullptr, nullptr, L"Just a window class", nullptr};
    ::RegisterClassExW( &wc );
    HWND hwnd = ::CreateWindowExW( WS_EX_TOPMOST, wc.lpszClassName, L"Just a window", WS_POPUP, 0, 0,
                                   GetSystemMetrics( SM_CXSCREEN ), GetSystemMetrics( SM_CYSCREEN ),
                                   nullptr, nullptr, wc.hInstance, nullptr );

    SetLayeredWindowAttributes( hwnd, RGB( 0, 0, 0 ), 255, LWA_ALPHA );
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea( hwnd, &margin );

    if ( !create_device_d3d( hwnd ) )
    {
        cleanup_device_d3d( );
        ::UnregisterClassW( wc.lpszClassName, wc.hInstance );
        return;
    }

    ::ShowWindow( hwnd, SW_SHOWDEFAULT );
    ::UpdateWindow( hwnd );

    ImGui::CreateContext( );

    ImGui::StyleColorsDark( );

    std::ifstream f("default.theme");
    if (f.good()) {
        theme.load("default.theme");
    } else {
        theme.save("default.theme");
    }


    ImGui_ImplWin32_Init( hwnd );
    ImGui_ImplDX11_Init( d3d_device, d3d_device_context );

    ImVec4 clear_color = ImVec4( 0.f, 0.f, 0.f, 0.f );

    bool done = false;
    bool overlay_enabled = true;
    while ( !done )
    {
        if ( GetAsyncKeyState( VK_INSERT ) & 1 )
        {
            overlay_enabled = !overlay_enabled;
            SetWindowLong( hwnd, GWL_EXSTYLE, overlay_enabled ? WS_EX_TOPMOST :
                           ( WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED ) );
        }

        MSG msg;
        while ( ::PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) )
        {
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );
            if ( msg.message == WM_QUIT )
            {
                done = true;
                break;
            }
        }

        if ( resize_width != 0 && resize_height != 0 )
        {
            cleanup_render_target( );
            swap_chain->ResizeBuffers( 0, resize_width, resize_height, DXGI_FORMAT_UNKNOWN, 0 );
            resize_width = resize_height = 0;
            create_render_target( );
        }

        ImGui_ImplDX11_NewFrame( );
        ImGui_ImplWin32_NewFrame( );
        ImGui::NewFrame( );

        rescan.start_search( );

        // Only run game-dependent features if the datamodel has been found
        if (g_main::datamodel)
        {
            feature_handler.start( g_main::datamodel );
            noclip.run();

            if (vars::misc::show_workspace_viewer)
            {
                workspace_viewer.run();

                // Store original depth stencil state
                ID3D11DepthStencilState* p_old_depth_stencil_state = nullptr;
                UINT stencil_ref = 0;
                d3d_device_context->OMGetDepthStencilState(&p_old_depth_stencil_state, &stencil_ref);

                // Apply no-depth stencil state for drawing through walls
                d3d_device_context->OMSetDepthStencilState(no_depth_stencil_state, 0);

                workspace_viewer.draw_selected_instance_highlight(); // Call highlight function

                // Restore original depth stencil state
                d3d_device_context->OMSetDepthStencilState(p_old_depth_stencil_state, stencil_ref);
                if (p_old_depth_stencil_state) p_old_depth_stencil_state->Release(); // Release the retrieved state
            }
        }

        // Always run the menu
        if ( overlay_enabled )
        {
            menu.run_main_window( );
        }

        ImGui::Render( );

        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                                                    clear_color.z * clear_color.w, clear_color.w };

        d3d_device_context->OMSetRenderTargets( 1, &render_target_view, nullptr );
        d3d_device_context->ClearRenderTargetView( render_target_view, clear_color_with_alpha );
        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

        swap_chain->Present( 0, 0 );
    }

    ImGui_ImplDX11_Shutdown( );
    ImGui_ImplWin32_Shutdown( );
    ImGui::DestroyContext( );

    cleanup_device_d3d( );
    ::DestroyWindow( hwnd );
    ::UnregisterClassW( wc.lpszClassName, wc.hInstance );
}