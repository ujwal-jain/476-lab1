#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#define PI 3.141596

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "boundingsphere.h"

using namespace glm;

#define PRIMDIR vec2(0.0f, -1.0f)
#define HSPEED 0.2f
#define SLIME_SIZE 1.0f
#define TIME_LEFT 200.0f
#define SHRINK_SPEED 100.f

class slime {
public:
    vec3 pos;
    vec2 dir;
    float speed;
    float hspeed;
    boundingsphere sphere;
    float timeLeft;

    slime() {
        pos = vec3((float)(rand()%PLANE_SIZE) - HPL_SIZE, 0.0f, (float)(rand()%PLANE_SIZE) - HPL_SIZE);
        float angle = (float)(rand() % (2 * 314)) / 100.0f;
        dir = vec2(cos(angle), sin(angle));
        speed = ((float)(rand() % 20) + 2) / 20.0f * 2;
        hspeed = HSPEED;
        float radius = sqrt(pow(SLIME_SIZE / 2.0f, 2) + pow(SLIME_SIZE / 2.0f, 2) + pow(SLIME_SIZE / 2.0f, 2));
        sphere = boundingsphere(pos, radius);
        timeLeft = 0;
    }

    void update(float frametime) {
        if(timeLeft > 0) {
            timeLeft -= frametime * SHRINK_SPEED;
            return;
        }

        if(pos.y > 0.15f)
            hspeed = -HSPEED;
        else if(pos.y < 0.0f)
            hspeed = HSPEED;
        vec2 vel2 = -dir * speed * frametime;
        vec3 vel = vec3(vel2.x, hspeed * frametime, vel2.y);
        pos += vel;

        sphere.updateCenter(pos);
    }

    void startTimer() {
        timeLeft = TIME_LEFT;
    }

    mat4 getBody() {
        float dotprod = dot(dir, PRIMDIR);
        float det = dir.x*PRIMDIR.y - dir.y*PRIMDIR.x;
        float angle = atan2(det, dotprod);

        // Size of slime shrinks if the player has collided with it already
        mat4 S;
        if(timeLeft == 0)
            S = scale(mat4(1.0f), vec3(SLIME_SIZE, SLIME_SIZE, SLIME_SIZE));
        else
            S = scale(mat4(1.0f), vec3(SLIME_SIZE, SLIME_SIZE, SLIME_SIZE) * (float)timeLeft / TIME_LEFT);

        return translate(mat4(1), pos + vec3(0, -1.3f + 0.5, 0.0f))
            * rotate(mat4(1), angle, vec3(0, 1, 0))
            * S;
    }

    mat4 getLeftEye() {
        mat4 T_eye = translate(mat4(1), vec3(-0.25, 0.25, 0.5));
        mat4 S = scale(mat4(1), vec3(0.1, 0.1, 0.1));
        mat4 M = getBody() * T_eye;
        return M * S;
    }

    mat4 getRightEye() {
        mat4 T_eye = translate(mat4(1), vec3(0.25, 0.25, 0.5));
        mat4 S = scale(mat4(1), vec3(0.1, 0.1, 0.1));
        mat4 M = getBody() * T_eye;
        return M * S;
    }

    mat4 getSmile() {
        mat4 T_smile= translate(mat4(1), vec3(0, -0.25, 0.5));
        mat4 S = scale(mat4(1), vec3(0.5, 0.1, 0.1));
        mat4 M = getBody() * T_smile;
        return M * S;
    }
};