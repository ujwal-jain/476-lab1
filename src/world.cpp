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
    mat4 rotation;
public:
    int w, a, s, d;
    World() {
        w = a = s = d = 0;
        rotation = mat4(1);
    }

    void rotateWorld(float ftime) {
        if(w) {
            if (d) rotateXY1(ftime * WORLDROTSPEED);
            else if (a) rotateXY2(ftime * WORLDROTSPEED);
            else rotateX(ftime * WORLDROTSPEED);
        }
        else if(s) {
            if(a) rotateXY1(-ftime * WORLDROTSPEED);
            else if(d) rotateXY2(-ftime * WORLDROTSPEED);
            else rotateX(-ftime * WORLDROTSPEED);
        }
        else if(a) rotateY(ftime * WORLDROTSPEED);
        else if(d) rotateY(-ftime * WORLDROTSPEED);
    }

    mat4 getModel() {
        return rotation * scale(mat4(1), vec3(WORLDSCALE));
    }

private:

    // if user presses w or s, rotate world over x axis
    void rotateX(float angle) {
        rotation = rotate(mat4(1), angle, vec3(1, 0, 0)) * rotation;
    }

    // if user pressed a or d, rotate world over y axis
    void rotateY(float angle) {
        rotation = rotate(mat4(1), angle, vec3(0, 1, 0)) * rotation;
    }

    // if user presses w and d or s and a, rotate the world over <1,1,0>
    void rotateXY1(float angle) {
        rotation = rotate(mat4(1), angle, vec3(1, -1, 0)) * rotation;
    }

    // if user presses w and a or s and d, rotate the world over <1,-1,0>
    void rotateXY2(float angle) {
        rotation = rotate(mat4(1), angle, vec3(1, 1, 0)) * rotation;
    }
};