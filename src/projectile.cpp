#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"

#define PROJSPAWNRADIUS 16.0f
#define PROJSPAWNRADIUS2 (PROJSPAWNRADIUS*PROJSPAWNRADIUS)
#define PROJRADIUS 1.0f
#define PROJSCALE 0.2f
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
    boundingsphere sphere;
    float lifespan;
    int shoot;

    Projectile() {
        // Some quick math to generate a random initial position of a projectile
        // Initialize position of the projectile to be anywhere on the sphere
        float x = (rand() % (int)(PROJSPAWNRADIUS * 20)) / 10.0f - PROJSPAWNRADIUS;
        // x^2 + y^2 + z^2 = r^2
        // if x is known, y^2 + z^2 = r^2 - x^2
        // generate y in range -sqrt(r^2 - x^2) to sqrt(r^2 - x^2)
        float maxy = sqrt(PROJSPAWNRADIUS2 - x*x);
        float y = (rand() % (int)(maxy * 20)) / 10.0f - maxy;
        // z = sqrt(r^2 - x^2 - y^2)
        float z = sqrt(PROJSPAWNRADIUS2 - x*x - y*y);

        pos = vec3(x, y, z);
        rot = mat4(1);
        w = a = s = d = 0;
        shoot = 0;

        rotAxis = vec3(0, 1, 0);

        sphere = boundingsphere(pos, PROJRADIUS);
        lifespan = 360.f;
    }

    Projectile(vec3 pos, vec3 dir)
    {
        this->pos = pos;
        rot = mat4(1);
        w = a = s = d = 0;
        shoot = 0;

        rotAxis = cross(pos, dir);
        sphere = boundingsphere(pos, PROJRADIUS);
        // 360 is entire rotation + a bit
        lifespan = 700;
    }

    mat4 getModel() const {
        return
            (rot
            * translate(mat4(1), pos))
            * translate(mat4(1), vec3(0, 0, 0))
            * scale(mat4(1), vec3(PROJSCALE));
    }

    void rotateProj(float frametime) {
        rot *= rotate(mat4(1), frametime * PROJROTSPEED, rotAxis);
        deteriorateProjectile();
    }

    void deteriorateProjectile() {
        lifespan -= 1;
    }
};
#endif