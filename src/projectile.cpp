#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"

#define WORLDROTSPEED 1.f
#define WORLDSCALE 15

using namespace std;
using namespace glm;


class Projectile
{
public:
    int w, a, s, d;
    int shoot;
    vec3 pos;
    mat4 rot;
    mat4 worldRot;

    Projectile()
    {
        pos = vec3(-1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2))), 0, 20);
        rot = mat4(1);
        worldRot = mat4(1);
        w = a = s = d = 0;
        
        shoot = 0;
    }

    mat4 getModel(float ttime) const {
        return 
            worldRot
            * rotate(mat4(1.0f), radians(ttime * 20), vec3(1, 0, 0))
            * translate(mat4(1), pos)
            * scale(mat4(1), vec3(PLAYERSCALE));
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

private:

    // if user presses w or s, rotate world over x axis
    void rotateX(float angle) {
        worldRot = rotate(mat4(1), angle, vec3(1, 0, 0)) * worldRot;
    }

    // if user pressed a or d, rotate world over y axis
    void rotateY(float angle) {
        worldRot = rotate(mat4(1), angle, vec3(0, 1, 0)) * worldRot;
    }

    // if user presses w and d or s and a, rotate the world over <1,1,0>
    void rotateXY1(float angle) {
        worldRot = rotate(mat4(1), angle, vec3(1, -1, 0)) * worldRot;
    }

    // if user presses w and a or s and d, rotate the world over <1,-1,0>
    void rotateXY2(float angle) {
        worldRot = rotate(mat4(1), angle, vec3(1, 1, 0)) * worldRot;
    }
    
};