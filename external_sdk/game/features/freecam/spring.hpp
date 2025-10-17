#pragma once
#include "core.hpp"
#include <cmath>

template <typename T>
class Spring {
public:
    T position;
    T velocity;
    T target;

    float stiffness;
    float damping;

    Spring(float stiffness, float damping) {
        this->stiffness = stiffness;
        this->damping = damping;
    }

    void update(float dt) {
        T force = (target - position) * stiffness;
        velocity = velocity + force * dt;
        velocity = velocity - velocity * damping * dt;
        position = position + velocity * dt;
    }

    void reset(T new_position) {
        position = new_position;
        target = new_position;
        velocity = T(); // Zero velocity
    }
};
