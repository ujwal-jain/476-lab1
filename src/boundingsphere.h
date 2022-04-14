#define STB_IMAGE_IMPLEMENTATION
#include "MatrixStack.h"

#ifndef BOUNDINGSPHERE_H
#define BOUNDINGSPHERE_H

using namespace glm;

class boundingsphere {
public:
    vec3 center;
    float radius;

    boundingsphere() {
        center = vec3(0.0f, 0.0f, 0.0f);
        radius = 0.0f;
    }

    boundingsphere(vec3 center, float radius) {
        this->center = center;
        this->radius = radius;
    }

    void updateCenter(vec3 center) {
        this->center = center;
    }
};
#endif