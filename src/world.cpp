#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#define WORLDROTSPEED 1.f
#define WORLDSCALE 15
#ifndef PI
#define PI 3.14159265358979323846
#endif

using namespace glm;

class World {

public:
    mat4 rotation;
    int w, a, s, d;
    World() {
        w = a = s = d = 0;
        rotation = mat4(1);
    }

    mat4 getModel() {
        return rotation * scale(mat4(1), vec3(WORLDSCALE));
    }
};