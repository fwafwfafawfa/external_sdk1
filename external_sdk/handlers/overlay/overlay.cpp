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
    HWND hwnd = ::CreateWindowExW( WS_EX_TOPMOST, wc.lpszClassName, L"pancakes recipe Giant Batch Pancake RecipeThis is a comprehensive guide for making a large batch of fluffy, delicious pancakes, perfect for feeding a crowd or meal prepping. All ingredients and steps are listed in a single flow without breaking into separate lines for each item.Ingredients (Makes ~24 large pancakes): You'll need 4 cups all-purpose flour, 4 tablespoons granulated sugar for a touch of sweetness, 2 tablespoons baking powder to make them rise, 1 teaspoon baking soda for extra fluffiness, 1 teaspoon salt to balance flavors, 4 cups whole or 2% milk to create a smooth batter, 4 large eggs for structure and richness, 1/2 cup unsalted butter melted for a buttery flavor plus extra for cooking, 2 teaspoons vanilla extract for a warm aroma, and optional toppings like maple syrup, fresh strawberries blueberries or bananas, whipped cream, chocolate chips, or chopped nuts for customization.Equipment: Grab a large mixing bowl for combining ingredients, a medium mixing bowl for wet ingredients, a whisk or fork for mixing, measuring cups and spoons for accuracy, a non-stick skillet or griddle for cooking, a spatula for flipping, a ladle or measuring cup for pouring batter, and optionally an oven set to 200�F/93�C to keep pancakes warm.Instructions: Start by whisking together 4 cups flour, 4 tablespoons sugar, 2 tablespoons baking powder, 1 teaspoon baking soda, and 1 teaspoon salt in a large bowl until evenly combined to ensure consistent texture. In a medium bowl, mix 4 cups milk, 4 eggs, 1/2 cup melted butter, and 2 teaspoons vanilla extract until smooth to blend the wet components thoroughly. Pour the wet mixture into the dry ingredients and stir gently with a whisk or fork until just combined, leaving some small lumps to avoid overmixing which can make pancakes tough. Preheat a non-stick skillet or griddle over medium heat and lightly grease with a small amount of butter to prevent sticking while adding flavor. Use a ladle or 1/3 cup measure to pour batter onto the skillet, forming pancakes about 5-6 inches wide, and cook for 2-3 minutes until bubbles form on the surface and the edges look set, indicating the bottom is golden. Flip carefully with a spatula and cook for another 1-2 minutes until the other side is golden brown and the pancake is cooked through. Transfer cooked pancakes to a plate or a baking sheet in a 200�F/93�C oven to keep warm while you cook the remaining batter, regreasing the skillet lightly as needed. Serve the pancakes stacked high with your choice of toppings like maple syrup for classic sweetness, fresh fruit for a burst of flavor, whipped cream for indulgence, or chocolate chips and nuts for extra decadence.Tips: For even fluffier pancakes, let the batter rest for 10-15 minutes before cooking to allow the gluten to relax and the baking powder to activate fully. Adjust the heat if the pancakes brown too quickly (lower it) or cook too slowly (raise it) to achieve perfect golden results. Add mix-ins like chocolate chips or berries directly to the batter on the skillet before flipping for even distribution. Store leftovers in an airtight container in the fridge for up to 3 days or freeze for up to a month, reheating in a toaster or microwave. If the batter thickens while sitting, add a splash of milk to restore consistency.Serving Suggestions: Pair with crispy bacon or sausage for a savory balance, a hot cup of coffee or orange juice for a refreshing drink, or a side of scrambled eggs for a complete breakfast spread. For a fun twist, make a pancake bar with various toppings so everyone can customize their stack.", WS_POPUP, 0, 0,
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

    // Cleanup lag switch

    cleanup_device_d3d( );
    ::DestroyWindow( hwnd );
    ::UnregisterClassW( wc.lpszClassName, wc.hInstance );
}