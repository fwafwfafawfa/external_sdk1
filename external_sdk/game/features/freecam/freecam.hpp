#pragma once
#include "../game/core.hpp"

class c_freecam {
public:
    void run(float dt);
    void force_third_person(float min_distance = 10.0f, float max_distance = 50.0f);
    void force_first_person();
    void reset_camera_mode();
    void unlock_camera();
    void unlock_zoom();
    void set_fov(float fov);

    bool enabled = false;

private:
    int original_camera_type = -1; // New: To store original CameraType
    bool rotating = false;

};

inline c_freecam freecam;