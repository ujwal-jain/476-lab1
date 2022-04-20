#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"

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
            * rotate(mat4(1), radians(90.0f), vec3(1, 0, 0))
            * scale(mat4(1), vec3(0.5, 0.5, 0.5));
    }

    void playerRotation() {
        if(!w && !a && !s && !d) return;
        float rotAngle = PI/4.0f;
        if(w && a) rotAngle *= 3;
        else if(w && d) rotAngle *= 1;
        else if(s && a) rotAngle *= 5;
        else if(s && d) rotAngle *= 7;
        else if(w) rotAngle *= 2;
        else if(s) rotAngle *= 6;
        else if(a) rotAngle *= 4;
        else if(d) rotAngle *= 0;
        rot = rotate(mat4(1), rotAngle - PI / 4, vec3(0, 0, 1));

    }

    mat4 camera()
    {
        return translate(mat4(1), vec3(0, 0, -30));
    }
};