#include "freecam.hpp"
#include "../../../handlers/vars.hpp"
#include "../../../game/offsets/offsets.hpp"
#include "../../../game/core.hpp"
#include "../../../addons/kernel/memory.hpp"
#include <Windows.h>
#include <iostream>
#include <cmath>

static bool init = false;
static uintptr_t saved_primitive = 0;
static vector stored_pos = { 0, 0, 0 };
static float yaw = 0.0f;
static float pitch = 0.0f;
static bool mouse_down = false;
static POINT locked_cursor = { 0, 0 };

// Cache these so we don't search every frame
static uintptr_t cached_workspace = 0;
static uintptr_t cached_camera = 0;

void c_freecam::set_fov(float fov_degrees)
{
    uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
    if (!workspace) return;

    uintptr_t camera = memory->read<uintptr_t>(workspace + offsets::Workspace::CurrentCamera);
    if (!camera) return;

    const float PI = 3.14159265358979f;
    float fov_radians = fov_degrees * (PI / 180.0f);

    memory->write<float>(camera + offsets::FOV, fov_radians);
}

void c_freecam::unlock_zoom()
{
    if (!g_main::localplayer) return;

    memory->write<float>(g_main::localplayer + offsets::CameraMinZoomDistance, 0.5f);
    memory->write<float>(g_main::localplayer + offsets::CameraMaxZoomDistance, 9999.0f);
    memory->write<int>(g_main::localplayer + offsets::CameraMode, 0);
}

void c_freecam::unlock_camera()
{
    if (!g_main::localplayer) return;

    memory->write<float>(g_main::localplayer + offsets::CameraMinZoomDistance, 0.5f);
    memory->write<float>(g_main::localplayer + offsets::CameraMaxZoomDistance, 9999.0f);
}

void c_freecam::reset_camera_mode()
{
    if (!g_main::localplayer) return;

    uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
    if (!workspace) return;

    uintptr_t cam = memory->read<uintptr_t>(workspace + offsets::Workspace::CurrentCamera);
    if (!cam) return;

    uintptr_t character = core.find_first_child(workspace, core.get_instance_name(g_main::localplayer));
    if (!character) return;

    uintptr_t humanoid = core.find_first_child_class(character, "Humanoid");
    if (!humanoid) return;

    memory->write<uintptr_t>(cam + offsets::CameraSubject, humanoid);
    memory->write<int>(cam + offsets::CameraType, 0);
    memory->write<float>(g_main::localplayer + offsets::CameraMinZoomDistance, 0.5f);
    memory->write<float>(g_main::localplayer + offsets::CameraMaxZoomDistance, 400.0f);
}

void c_freecam::run(float dt)
{
    if (!g_main::localplayer) return;

    // Cache workspace (only search once)
    if (!cached_workspace || !cached_camera)
    {
        cached_workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
        if (!cached_workspace) return;

        cached_camera = memory->read<uintptr_t>(cached_workspace + offsets::Workspace::CurrentCamera);
        if (!cached_camera) return;
    }

    uintptr_t cam = cached_camera;

    // ========== ENABLE ==========
    if (vars::freecam::toggled && !init)
    {
        uintptr_t character = core.find_first_child(cached_workspace, core.get_instance_name(g_main::localplayer));

        if (character)
        {
            uintptr_t rootpart = core.find_first_child(character, "HumanoidRootPart");
            if (rootpart)
            {
                saved_primitive = memory->read<uintptr_t>(rootpart + offsets::Primitive);
                if (saved_primitive)
                {
                    uint8_t anchored_byte = memory->read<uint8_t>(saved_primitive + offsets::Anchored);
                    anchored_byte |= offsets::AnchoredMask;
                    memory->write<uint8_t>(saved_primitive + offsets::Anchored, anchored_byte);
                }
            }
        }

        stored_pos = memory->read<vector>(cam + offsets::CameraPos);
        yaw = 0.0f;
        pitch = 0.0f;

        std::cout << "[FREECAM] Enabled" << std::endl;
        init = true;
    }

    // ========== DISABLE ==========
    if (!vars::freecam::toggled && init)
    {
        uintptr_t character = core.find_first_child(cached_workspace, core.get_instance_name(g_main::localplayer));

        if (character)
        {
            uintptr_t humanoid = core.find_first_child_class(character, "Humanoid");
            if (humanoid)
            {
                memory->write<uintptr_t>(cam + offsets::CameraSubject, humanoid);
            }
        }

        memory->write<int>(cam + offsets::CameraType, 0);

        if (saved_primitive)
        {
            uint8_t anchored_byte = memory->read<uint8_t>(saved_primitive + offsets::Anchored);
            anchored_byte &= ~offsets::AnchoredMask;
            memory->write<uint8_t>(saved_primitive + offsets::Anchored, anchored_byte);
            saved_primitive = 0;
        }

        if (mouse_down)
        {
            ShowCursor(TRUE);
            mouse_down = false;
        }

        // Clear cache on disable
        cached_workspace = 0;
        cached_camera = 0;

        std::cout << "[FREECAM] Disabled" << std::endl;
        init = false;
        return;
    }

    // ========== FREECAM ACTIVE ==========
    if (vars::freecam::toggled)
    {
        // Set scriptable camera mode
        memory->write<int>(cam + offsets::CameraType, 6);
        memory->write<uintptr_t>(cam + offsets::CameraSubject, 0);

        // Mouse look (right click hold)
        bool rmb = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

        if (rmb && !mouse_down)
        {
            GetCursorPos(&locked_cursor);
            ShowCursor(FALSE);
            mouse_down = true;
        }
        else if (!rmb && mouse_down)
        {
            ShowCursor(TRUE);
            mouse_down = false;
        }

        if (mouse_down)
        {
            POINT cur;
            GetCursorPos(&cur);

            int dx = cur.x - locked_cursor.x;
            int dy = cur.y - locked_cursor.y;

            if (dx != 0 || dy != 0)
            {
                yaw -= dx * 0.003f;
                pitch += dy * 0.003f;
                pitch = fmaxf(-1.55f, fminf(1.55f, pitch));
                SetCursorPos(locked_cursor.x, locked_cursor.y);
            }
        }

        // Arrow key look
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) yaw -= 0.03f;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) yaw += 0.03f;
        if (GetAsyncKeyState(VK_UP) & 0x8000) pitch -= 0.03f;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000) pitch += 0.03f;
        pitch = fmaxf(-1.55f, fminf(1.55f, pitch));

        // Speed
        float speed = vars::freecam::speed;
        if (GetAsyncKeyState(VK_SHIFT) & 0x8000) speed *= 0.25f;

        // Build rotation matrix
        float cy = cosf(yaw);
        float sy = sinf(yaw);
        float cp = cosf(pitch);
        float sp = sinf(pitch);

        Matrix3 rot;
        rot.data[0] = cy;
        rot.data[1] = 0;
        rot.data[2] = -sy;
        rot.data[3] = sy * sp;
        rot.data[4] = cp;
        rot.data[5] = cy * sp;
        rot.data[6] = sy * cp;
        rot.data[7] = -sp;
        rot.data[8] = cy * cp;

        // Direction vectors
        vector forward = { rot.data[2], rot.data[5], rot.data[8] };
        vector right = { rot.data[0], rot.data[3], rot.data[6] };

        // Movement
        if (GetAsyncKeyState('W') & 0x8000)
        {
            stored_pos.x -= forward.x * speed;
            stored_pos.y -= forward.y * speed;
            stored_pos.z -= forward.z * speed;
        }
        if (GetAsyncKeyState('S') & 0x8000)
        {
            stored_pos.x += forward.x * speed;
            stored_pos.y += forward.y * speed;
            stored_pos.z += forward.z * speed;
        }
        if (GetAsyncKeyState('A') & 0x8000)
        {
            stored_pos.x -= right.x * speed;
            stored_pos.z -= right.z * speed;
        }
        if (GetAsyncKeyState('D') & 0x8000)
        {
            stored_pos.x += right.x * speed;
            stored_pos.z += right.z * speed;
        }
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) stored_pos.y += speed;
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) stored_pos.y -= speed;

        // Write to camera
        memory->write<Matrix3>(cam + offsets::CameraRotation, rot);
        memory->write<vector>(cam + offsets::CameraPos, stored_pos);
    }
}