#include "freecam.hpp"
#include "../../../handlers/vars.hpp"
#include "../../../game/offsets/offsets.hpp"
#include "../../../game/core.hpp"
#include "../../../addons/kernel/memory.hpp"
#include <Windows.h>
#include <iostream>

static bool init = false;
static uintptr_t saved_primitive = 0;
static vector stored_pos = { 0, 0, 0 };
static float yaw = 0.0f;
static float pitch = 0.0f;
static bool mouse_down = false;
static POINT locked_cursor = { 0, 0 };

void c_freecam::run(float dt)
{
    if (!g_main::localplayer) return;

    uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
    if (!workspace) return;

    uintptr_t cam = memory->read<uintptr_t>(workspace + offsets::Camera);
    if (!cam) return;

    if (vars::freecam::toggled && !init)
    {
        uintptr_t character = core.find_first_child(workspace, core.get_instance_name(g_main::localplayer));

        if (character) {
            uintptr_t rootpart = core.find_first_child(character, "HumanoidRootPart");

            if (rootpart) {
                saved_primitive = memory->read<uintptr_t>(rootpart + offsets::Primitive);

                if (saved_primitive) {
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

    if (!vars::freecam::toggled && init)
    {
        std::cout << "[FREECAM] Disabling..." << std::endl;

        uintptr_t character = core.find_first_child(workspace, core.get_instance_name(g_main::localplayer));
        std::cout << "[FREECAM] Character: " << std::hex << character << std::dec << std::endl;

        if (character) {
            uintptr_t humanoid = core.find_first_child_class(character, "Humanoid");
            std::cout << "[FREECAM] Humanoid: " << std::hex << humanoid << std::dec << std::endl;

            if (humanoid) {
                memory->write<uintptr_t>(cam + offsets::CameraSubject, humanoid);
                std::cout << "[FREECAM] Set CameraSubject to Humanoid" << std::endl;
            }
        }

        memory->write<int>(cam + offsets::CameraType, 5);
        std::cout << "[FREECAM] Set CameraType to 5" << std::endl;

        if (saved_primitive) {
            uint8_t anchored_byte = memory->read<uint8_t>(saved_primitive + offsets::Anchored);
            std::cout << "[FREECAM] Anchored byte before: " << (int)anchored_byte << std::endl;
            anchored_byte &= ~offsets::AnchoredMask;
            memory->write<uint8_t>(saved_primitive + offsets::Anchored, anchored_byte);
            std::cout << "[FREECAM] Unanchored player" << std::endl;
            saved_primitive = 0;
        }

        if (mouse_down) {
            ShowCursor(TRUE);
            mouse_down = false;
        }

        std::cout << "[FREECAM] Disabled" << std::endl;

        init = false;
        return;
    }

    if (vars::freecam::toggled)
    {
        memory->write<int>(cam + offsets::CameraType, 6);
        memory->write<uintptr_t>(cam + offsets::CameraSubject, 0);

        bool rmb = (GetAsyncKeyState(VK_RBUTTON) & 0x8000);

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

        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) yaw += 0.03f;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) yaw -= 0.03f;
        if (GetAsyncKeyState(VK_UP) & 0x8000) pitch += 0.03f;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000) pitch -= 0.03f;

        pitch = fmaxf(-1.55f, fminf(1.55f, pitch));

        float speed = vars::freecam::speed;
        if (GetAsyncKeyState(VK_SHIFT) & 0x8000) speed *= 0.25f;

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

        vector forward = { rot.data[2], rot.data[5], rot.data[8] };
        vector right = { rot.data[0], rot.data[3], rot.data[6] };

        if (GetAsyncKeyState('W') & 0x8000) {
            stored_pos.x -= forward.x * speed;
            stored_pos.y -= forward.y * speed;
            stored_pos.z -= forward.z * speed;
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            stored_pos.x += forward.x * speed;
            stored_pos.y += forward.y * speed;
            stored_pos.z += forward.z * speed;
        }
        if (GetAsyncKeyState('A') & 0x8000) {
            stored_pos.x -= right.x * speed;
            stored_pos.z -= right.z * speed;
        }
        if (GetAsyncKeyState('D') & 0x8000) {
            stored_pos.x += right.x * speed;
            stored_pos.z += right.z * speed;
        }
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) stored_pos.y += speed;
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) stored_pos.y -= speed;

        memory->write<Matrix3>(cam + offsets::CameraRotation, rot);
        memory->write<vector>(cam + offsets::CameraPos, stored_pos);
    }
}
