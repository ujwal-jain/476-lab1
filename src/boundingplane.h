#ifndef BOUNDINGPLANE_H
#define BOUNDINGPLANE_H
#include "MatrixStack.h"

using namespace glm;

class boundingplane {
    vec4 constants;

public:
    vec3 getNormal() {
        return vec3(constants.x, constants.y, constants.z);
    }

    float getD() {
        return constants.w;
    }

    boundingplane(float a, float b, float c, float d) {
        constants = vec4(a, b, c, d);
    }
};

#endif