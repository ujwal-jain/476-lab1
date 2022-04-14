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

class slime {
public:
    vec3 pos;
    vec2 dir;
    slime() {
        pos = vec3(0, 0, -3);
        dir = vec2(0, -1);
    }

    mat4 getBody() {
        float dotprod = dot(dir, PRIMDIR);
        float det = dir.x*PRIMDIR.y - dir.y*PRIMDIR.x;
        float angle = atan2(det, dotprod);

        return translate(mat4(1), pos) * rotate(mat4(1), angle, vec3(0, 1, 0)) * scale(mat4(1), vec3(3, 3, 3));
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