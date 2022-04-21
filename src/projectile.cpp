#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"

#define PROJSPAWNRADIUS 16.0f
#define PROJSPAWNRADIUS2 (PROJSPAWNRADIUS*PROJSPAWNRADIUS)
#define PROJRADIUS 1.0f
#define PROJSCALE 0.1f

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
    }

    Projectile(vec3 targetLocation)
    {
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

        rotAxis = cross(pos, targetLocation - pos);
        sphere = boundingsphere(pos, PROJRADIUS);
    }

    mat4 getModel() const {
        return
                (rot
            * translate(mat4(1), pos))
            * scale(mat4(1), vec3(PROJSCALE));
    }

    void rotateProj(float frametime) {
        // Since the world has a separate rotation, we need to rotate the rotAxis by the world rotation
//        vec4 realRotAxis = vec4(rotAxis, 1) * worldRotation;
//        rot *= rotate(mat4(1), frametime, vec3(realRotAxis));
        rot *= rotate(mat4(1), frametime, rotAxis);
    }
};