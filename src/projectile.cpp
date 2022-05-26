#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"

#define PROJSPAWNRADIUS 13.6f
#define PROJSPAWNRADIUS2 (PROJSPAWNRADIUS*PROJSPAWNRADIUS)
#define PROJRADIUS 0.3f
#define PROJSCALE 0.12f
#define PROJROTSPEED 1.2f

#ifndef PROJECTILE
#define PROJECTILE

using namespace std;
using namespace glm;


class Projectile
{
public:
    int w, a, s, d;
    vec3 pos;
    mat4 rot;
    vec3 rotAxis;
    boundingsphere hitbox;
    int lifespan;
    int graceTimeLeft;

    Projectile(vec3 pos, vec3 dir, int gracePeriodTime)
    {
        this->pos = pos;
        rot = mat4(1);
        w = a = s = d = 0;

        rotAxis = cross(pos, dir);
        hitbox = boundingsphere(pos, PROJRADIUS);
        // 360 is entire rotation + a bit
        lifespan = 500;

        graceTimeLeft = gracePeriodTime;
    }

    mat4 getModel() const {
        return translate(mat4(1), pos * vec3(0.90)) * scale(mat4(1), vec3(PROJSCALE));
    }

    void rotateProj(float frametime) {
        rot *= rotate(mat4(1), frametime * PROJROTSPEED, rotAxis);
        vec4 newPos = vec4(pos, 1) * rotate(mat4(1), -frametime * PROJROTSPEED, rotAxis);
        pos = vec3(newPos);
        hitbox.center = pos;

        if(graceTimeLeft > 0) {
            graceTimeLeft--;
        }
        deteriorateProjectile();
    }

    void deteriorateProjectile() {
        lifespan -= 1;
    }
};
#endif