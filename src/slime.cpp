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

using namespace glm;

#define PRIMDIR vec2(0.0f, -1.0f)
#define HSPEED 0.2f

class slime {
public:
    vec3 pos;
    vec2 dir;
    float speed;
    float hspeed;
    slime() {
        pos = vec3((float)(rand()%PLANE_SIZE) - HPL_SIZE, 0.0f, (float)(rand()%PLANE_SIZE) - HPL_SIZE);
        float angle = (float)(rand() % (2 * 314)) / 100.0f;
        dir = vec2(cos(angle), sin(angle));
        speed = ((float)(rand() % 20) + 2) / 20.0f * 2;
        hspeed = HSPEED;
    }

    void update(float frametime) {
        if(pos.y > 0.15f)
            hspeed = -HSPEED;
        else if(pos.y < 0.0f)
            hspeed = HSPEED;
        if(pos.x > HPL_SIZE)
            dir.x = -dir.x;
        else if(pos.x < -HPL_SIZE)
            dir.x = -dir.x;
        if(pos.z > HPL_SIZE)
            dir.y = -dir.y;
        else if(pos.z < -HPL_SIZE)
            dir.y = -dir.y;
        vec2 vel2 = -dir * speed * frametime;
        vec3 vel = vec3(vel2.x, hspeed * frametime, vel2.y);
        pos += vel;
    }

    mat4 getBody() {
        float dotprod = dot(dir, PRIMDIR);
        float det = dir.x*PRIMDIR.y - dir.y*PRIMDIR.x;
        float angle = atan2(det, dotprod);

        return translate(mat4(1), pos + vec3(0, -1.3f + 0.5, 0.0f))
            * rotate(mat4(1), angle, vec3(0, 1, 0))
            * scale(mat4(1), vec3(1, 1, 1));
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