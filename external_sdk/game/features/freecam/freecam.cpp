#include "freecam.hpp"
#include "../../../main.hpp"
#include <Windows.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Helper Functions ---
matrix create_identity_matrix() {
    matrix m = {};
    m.m[0][0] = m.m[1][1] = m.m[2][2] = m.m[3][3] = 1.0f;
    return m;
}

matrix multiply_matrices(const matrix& m1, const matrix& m2) {
    matrix result = {};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
        }
    }
    return result;
}

matrix create_yaw_matrix(float yaw) {
    matrix m = create_identity_matrix();
    m.m[0][0] = cos(yaw);
    m.m[0][2] = -sin(yaw);
    m.m[2][0] = sin(yaw);
    m.m[2][2] = cos(yaw);
    return m;
}

matrix create_pitch_matrix(float pitch) {
    matrix m = create_identity_matrix();
    m.m[1][1] = cos(pitch);
    m.m[1][2] = sin(pitch);
    m.m[2][1] = -sin(pitch);
    m.m[2][2] = cos(pitch);
    return m;
}

matrix lerp_matrix(const matrix& a, const matrix& b, float t) {
    matrix result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = a.m[i][j] * (1.0f - t) + b.m[i][j] * t;
        }
    }
    return result;
}

// --- c_freecam Implementation ---
void c_freecam::run(float dt) {
    uintptr_t workspace = core.find_first_child_class(g_main::datamodel, "Workspace");
    if (!workspace) {
        if (rotating) {
            ShowCursor(true);
            rotating = false;
        }
        return;
    }
    uintptr_t camera_ptr = driver.read<uintptr_t>(workspace + offsets::Camera);
    if (!camera_ptr) {
        if (rotating) {
            ShowCursor(true);
            rotating = false;
        }
        return;
    }

    if (!enabled) {
        if (original_subject != 0) {
            driver.write<uintptr_t>(camera_ptr + offsets::CameraSubject, original_subject);
            original_subject = 0;
            if (rotating) {
                ShowCursor(true);
                rotating = false;
            }
        }
        return;
    }

    if (original_subject == 0) {
        original_subject = driver.read<uintptr_t>(camera_ptr + offsets::CameraSubject);
        driver.write<uintptr_t>(camera_ptr + offsets::CameraSubject, 0);
    }

    matrix current_cframe = driver.read<matrix>(camera_ptr + offsets::CFrame);
    matrix final_cframe = current_cframe;

    // --- Rotation ---
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
        static POINT last_pos = {0,0};
        if (!rotating) {
            rotating = true;
            GetCursorPos(&last_pos);
            ShowCursor(false);
        }
        
        POINT current_pos;
        GetCursorPos(&current_pos);
        int deltaX = current_pos.x - last_pos.x;
        int deltaY = current_pos.y - last_pos.y;
        SetCursorPos(last_pos.x, last_pos.y);

        if (deltaX != 0 || deltaY != 0) {
            float yaw_change = -deltaX * vars::freecam::sensitivity;
            float pitch_change = -deltaY * vars::freecam::sensitivity;

            vector pos = {final_cframe.m[0][3], final_cframe.m[1][3], final_cframe.m[2][3]};
            final_cframe.m[0][3] = final_cframe.m[1][3] = final_cframe.m[2][3] = 0;

            matrix yaw_rot = create_yaw_matrix(yaw_change);
            matrix pitch_rot = create_pitch_matrix(pitch_change);

            final_cframe = multiply_matrices(yaw_rot, final_cframe);
            final_cframe = multiply_matrices(final_cframe, pitch_rot);

            final_cframe.m[0][3] = pos.x; final_cframe.m[1][3] = pos.y; final_cframe.m[2][3] = pos.z;
        }
    } else {
        if (rotating) {
            rotating = false;
            ShowCursor(true);
        }
    }

    // --- Movement ---
    float speed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? vars::freecam::speed * 2.0f : vars::freecam::speed;
    float frame_speed = speed * dt;

    vector local_move_dir = {};
    if (GetAsyncKeyState('W') & 0x8000) local_move_dir.z -= frame_speed;
    if (GetAsyncKeyState('S') & 0x8000) local_move_dir.z += frame_speed;
    if (GetAsyncKeyState('A') & 0x8000) local_move_dir.x -= frame_speed;
    if (GetAsyncKeyState('D') & 0x8000) local_move_dir.x += frame_speed;

    matrix translation = create_identity_matrix();
    translation.m[0][3] = local_move_dir.x;
    translation.m[2][3] = local_move_dir.z;
    final_cframe = multiply_matrices(final_cframe, translation);

    if (GetAsyncKeyState(VK_SPACE) & 0x8000) final_cframe.m[1][3] += frame_speed;
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) final_cframe.m[1][3] -= frame_speed;

    // --- Write to game ---
    matrix lerped_frame = lerp_matrix(current_cframe, final_cframe, 0.3f);
    driver.write<matrix>(camera_ptr + offsets::CFrame, lerped_frame);
}
