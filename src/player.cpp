#include <iostream>

#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"
using namespace std;
using namespace glm;

#define PLAYERSCALE 0.3f
#ifndef PI
#define PI 3.14159265f
#endif

class Player
{
public:
    int w, a, s, d;
    vec3 pos;
    mat4 rot;

    Player()
    {
        pos = vec3(0, 0, 20);
        rot = mat4(1);
        w = a = s = d = 0;
    }

    mat4 getModel() const {
        return translate(mat4(1), pos)
               * rot
               * rotate(mat4(1), PI / 2, vec3(1, 0, 0))
               * scale(mat4(1), vec3(PLAYERSCALE));
    }

    void playerRotation(GLFWwindow *window, double xpos, double ypos) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float xposFloat = static_cast<float>(xpos);
        float yposFloat = static_cast<float>(ypos);

        vec2 cursor = normalize(vec2(xposFloat - (width / 2), -(yposFloat - (height / 2))));

        printf("%f %f\n", cursor.x, cursor.y);
        rot = rotate(mat4(1), atan(cursor.y, cursor.x) - PI/4.0f, vec3(0, 0, 1));
    }

    mat4 camera()
    {
        return translate(mat4(1), vec3(0, 0, -40));
    }
};