#include "freecam.hpp"
#include "../../../main.hpp"
#include <Windows.h>
#include <cmath>
#include "../../../handlers/utility/utility.hpp"
#include "../../../handlers/vars.hpp" // Explicitly include vars.hpp for vector struct

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Helper Functions ---
CFrame create_identity_cframe() {
    CFrame cf = {};
    cf.R00 = 1.0f; cf.R01 = 0.0f; cf.R02 = 0.0f;
    cf.R10 = 0.0f; cf.R11 = 1.0f; cf.R12 = 0.0f;
    cf.R20 = 0.0f; cf.R21 = 0.0f; cf.R22 = 1.0f;
    cf.X = 0.0f; cf.Y = 0.0f; cf.Z = 0.0f;
    return cf;
}

CFrame multiply_cframes(const CFrame& c1, const CFrame& c2) {
    CFrame result;
    result.R00 = c1.R00 * c2.R00 + c1.R01 * c2.R10 + c1.R02 * c2.R20;
    result.R01 = c1.R00 * c2.R01 + c1.R01 * c2.R11 + c1.R02 * c2.R21;
    result.R02 = c1.R00 * c2.R02 + c1.R01 * c2.R12 + c1.R02 * c2.R22;

    result.R10 = c1.R10 * c2.R00 + c1.R11 * c2.R10 + c1.R12 * c2.R20;
    result.R11 = c1.R10 * c2.R01 + c1.R11 * c2.R11 + c1.R12 * c2.R21;
    result.R12 = c1.R10 * c2.R02 + c1.R11 * c2.R12 + c1.R12 * c2.R22;

    result.R20 = c1.R20 * c2.R00 + c1.R21 * c2.R10 + c1.R22 * c2.R20;
    result.R21 = c1.R20 * c2.R01 + c1.R21 * c2.R11 + c1.R22 * c2.R21;
    result.R22 = c1.R20 * c2.R02 + c1.R21 * c2.R12 + c1.R22 * c2.R22;

    result.X = c1.R00 * c2.X + c1.R01 * c2.Y + c1.R02 * c2.Z + c1.X;
    result.Y = c1.R10 * c2.X + c1.R11 * c2.Y + c1.R12 * c2.Z + c1.Y;
    result.Z = c1.R20 * c2.X + c1.R21 * c2.Y + c1.R22 * c2.Z + c1.Z;
    return result;
}

CFrame create_yaw_cframe(float yaw) {
    CFrame cf = create_identity_cframe();
    float cos_yaw = cos(yaw);
    float sin_yaw = sin(yaw);
    cf.R00 = cos_yaw; cf.R02 = -sin_yaw;
    cf.R20 = sin_yaw; cf.R22 = cos_yaw;
    return cf;
}

CFrame create_pitch_cframe(float pitch) {
    CFrame cf = create_identity_cframe();
    float cos_pitch = cos(pitch);
    float sin_pitch = sin(pitch);
    cf.R11 = cos_pitch; cf.R12 = sin_pitch;
    cf.R21 = -sin_pitch; cf.R22 = cos_pitch;
    return cf;
}

CFrame lerp_cframe(const CFrame& a, const CFrame& b, float t) {
    CFrame result;
    result.R00 = a.R00 * (1.0f - t) + b.R00 * t;
    result.R01 = a.R01 * (1.0f - t) + b.R01 * t;
    result.R02 = a.R02 * (1.0f - t) + b.R02 * t;

    result.R10 = a.R10 * (1.0f - t) + b.R10 * t;
    result.R11 = a.R11 * (1.0f - t) + b.R11 * t;
    result.R12 = a.R12 * (1.0f - t) + b.R12 * t;

    result.R20 = a.R20 * (1.0f - t) + b.R20 * t;
    result.R21 = a.R21 * (1.0f - t) + b.R21 * t;
    result.R22 = a.R22 * (1.0f - t) + b.R22 * t;

    result.X = a.X * (1.0f - t) + b.X * t;
    result.Y = a.Y * (1.0f - t) + b.Y * t;
    result.Z = a.Z * (1.0f - t) + b.Z * t;
    return result;
}

// --- c_freecam Implementation ---
void c_freecam::run(float dt) {
    util.m_print("freecam::run() called"); // Debug print

    uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
    if (!workspace) {
        util.m_print("freecam: Workspace not found"); // Debug print
        if (rotating) {
            ShowCursor(true);
            rotating = false;
        }
        return;
    }
    uintptr_t camera_ptr = memory->read<uintptr_t>(workspace + offsets::Camera);
    if (!camera_ptr) {
        util.m_print("freecam: Camera pointer not found"); // Debug print
        if (rotating) {
            ShowCursor(true);
            rotating = false;
        }
        return;
    }
    util.m_print("freecam: Camera Ptr: 0x%llX", camera_ptr); // Debug print
    util.m_print("freecam: Current CameraType: %d", memory->read<int>(camera_ptr + offsets::CameraType)); // Debug print

    if (!enabled) {
        util.m_print("freecam: Not enabled"); // Debug print
        if (original_camera_type != -1) { // Check if a type was stored
            util.m_print("freecam: Restoring original camera type"); // Debug print
            memory->write<int>(camera_ptr + offsets::CameraType, original_camera_type);
            original_camera_type = -1; // Reset to indicate no type stored
            if (rotating) {
                ShowCursor(true);
                rotating = false;
            }
        }
        return;
    }

    if (original_camera_type == -1) { // Only set if not already set
        util.m_print("freecam: Setting CameraType to Scriptable (6)"); // Debug print
        original_camera_type = memory->read<int>(camera_ptr + offsets::CameraType);
        memory->write<int>(camera_ptr + offsets::CameraType, 6); // Scriptable
        util.m_print("freecam: CameraType set to: %d", memory->read<int>(camera_ptr + offsets::CameraType)); // Debug print
    }

    CFrame current_cframe = memory->read<CFrame>(camera_ptr + offsets::CFrame);
    util.m_print("freecam: Current CFrame: Pos(%.2f, %.2f, %.2f)", current_cframe.X, current_cframe.Y, current_cframe.Z); // Debug print
    util.m_print("freecam: Current CFrame: Rot(%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)",
        current_cframe.R00, current_cframe.R01, current_cframe.R02,
        current_cframe.R10, current_cframe.R11, current_cframe.R12,
        current_cframe.R20, current_cframe.R21, current_cframe.R22); // Debug print

    CFrame final_cframe = current_cframe;

    // --- Rotation ---
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
        static POINT last_pos = {0,0};
        if (!rotating) {
            rotating = true;
            GetCursorPos(&last_pos);
            ShowCursor(false);
            util.m_print("freecam: Started rotating"); // Debug print
        }
        
        POINT current_pos;
        GetCursorPos(&current_pos);
        int deltaX = current_pos.x - last_pos.x;
        int deltaY = current_pos.y - last_pos.y;
        SetCursorPos(last_pos.x, last_pos.y);

        if (deltaX != 0 || deltaY != 0) {
            float yaw_change = -deltaX * vars::freecam::sensitivity;
            float pitch_change = -deltaY * vars::freecam::sensitivity;
            util.m_print("freecam: Yaw: %.2f, Pitch: %.2f", yaw_change, pitch_change); // Debug print

            // Extract position before rotation
            vector pos = {final_cframe.X, final_cframe.Y, final_cframe.Z};
            final_cframe.X = final_cframe.Y = final_cframe.Z = 0; // Zero out position for rotation

            CFrame yaw_rot = create_yaw_cframe(yaw_change);
            CFrame pitch_rot = create_pitch_cframe(pitch_change);

            final_cframe = multiply_cframes(yaw_rot, final_cframe);
            final_cframe = multiply_cframes(final_cframe, pitch_rot);

            // Restore position after rotation
            final_cframe.X = pos.x; final_cframe.Y = pos.y; final_cframe.Z = pos.z;
        }
    } else {
        if (rotating) {
            rotating = false;
            ShowCursor(true);
            util.m_print("freecam: Stopped rotating"); // Debug print
        }
    }

    // --- Movement ---
    float speed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? vars::freecam::speed * 2.0f : vars::freecam::speed;
    float frame_speed = speed * dt;

    ::vector local_move_dir = {};
    if (GetAsyncKeyState('W') & 0x8000) local_move_dir.z -= frame_speed;
    if (GetAsyncKeyState('S') & 0x8000) local_move_dir.z += frame_speed;
    if (GetAsyncKeyState('A') & 0x8000) local_move_dir.x -= frame_speed;
    if (GetAsyncKeyState('D') & 0x8000) local_move_dir.x += frame_speed;
    util.m_print("freecam: Local Move Dir: X: %.2f, Y: %.2f, Z: %.2f", local_move_dir.x, local_move_dir.y, local_move_dir.z); // Debug print

    // Apply local movement to CFrame
    final_cframe.X += final_cframe.R00 * local_move_dir.x + final_cframe.R01 * local_move_dir.y + final_cframe.R02 * local_move_dir.z;
    final_cframe.Y += final_cframe.R10 * local_move_dir.x + final_cframe.R11 * local_move_dir.y + final_cframe.R12 * local_move_dir.z;
    final_cframe.Z += final_cframe.R20 * local_move_dir.x + final_cframe.R21 * local_move_dir.y + final_cframe.R22 * local_move_dir.z;

    if (GetAsyncKeyState(VK_SPACE) & 0x8000) final_cframe.Y += frame_speed;
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) final_cframe.Y -= frame_speed;
    util.m_print("freecam: Final CFrame Pos: X: %.2f, Y: %.2f, Z: %.2f", final_cframe.X, final_cframe.Y, final_cframe.Z); // Debug print

    // --- Write to game ---
    CFrame lerped_frame = lerp_cframe(current_cframe, final_cframe, 0.3f);
    memory->write<CFrame>(camera_ptr + offsets::CFrame, lerped_frame);
}
