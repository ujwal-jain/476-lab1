#include <iostream>

#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"

#define CAMSPEED 3.0f

using namespace std;
using namespace glm;

class Camera {
public:
    int w, a, s, d;
    vec3 realPos;
    vec3 camCenter;
    vec2 camOffset;
    vec2 camVelocity;
    vec3 camUp;
    vec3 camFwd;
    vec3 camRight;

    Camera() {
        w = a = s = d = 0;
        camCenter = vec3(0, 0, 30);
        camOffset = vec2(0, 0);
        camVelocity = vec2(0, 0);
        camUp = vec3(0, 1, 0);
        camRight = vec3(1, 0, 0);
        camFwd = vec3(0, 0, -1);
    }

    mat4 topCam() {
        vec3 xOffset = camRight * camOffset.x;
        vec3 yOffset = camUp * camOffset.y;
        realPos = camCenter + xOffset + yOffset;
        return lookAt(camCenter, vec3(0), camUp);
    }

    mat4 playerCam(vec3 playerPos) {
        return lookAt(playerPos - 12.f * camUp + 15.f * camFwd, playerPos, camFwd);
    }

    void update(float frametime, vec3 newPos, vec3 newUp, vec3 newRight, vec3 newFwd) {
        camCenter = newPos * 7.0f / 3.0f;
        camRight = newRight;
        camUp = newUp;
        camFwd = newFwd;

        if(w == s) {
            if(camOffset.y > 0)
                camVelocity.y = -sqrt(abs(camOffset.y)) * frametime;
            else
                camVelocity.y = sqrt(abs(camOffset.y)) * frametime;
        }
        else if(w)
            camVelocity.y = CAMSPEED * frametime;
        else if(s)
            camVelocity.y = -CAMSPEED * frametime;

        if(a == d)
            if(camOffset.x > 0)
                camVelocity.x = -sqrt(abs(camOffset.x)) * frametime;
            else
                camVelocity.x = sqrt(abs(camOffset.x)) * frametime;
        else if(a)
            camVelocity.x = CAMSPEED * frametime;
        else if(d)
            camVelocity.x = -CAMSPEED * frametime;

        camOffset += camVelocity;
        camOffset.x = clamp(camOffset.x, -5.0f, 5.0f);
        camOffset.y = clamp(camOffset.y, -5.0f, 5.0f);
    }
};