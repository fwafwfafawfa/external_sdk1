#pragma once
#include "../game/core.hpp"

class c_freecam {
public:
    void run(float dt);

    bool enabled = false;

private:
    int original_camera_type = -1; // New: To store original CameraType
    bool rotating = false;
};

inline c_freecam freecam;