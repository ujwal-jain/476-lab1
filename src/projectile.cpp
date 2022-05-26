#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"
#include "particleSys.h"
#include <iostream>

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
    particleSys *thePartSystem;
    vec3 color;

    Projectile(vec3 pos, vec3 dir, int gracePeriodTime, vec3 partColor)
    {
        this->pos = pos;
        rot = mat4(1);
        w = a = s = d = 0;

        rotAxis = cross(pos, dir);
        hitbox = boundingsphere(pos, PROJRADIUS);
        // 360 is entire rotation + a bit
        lifespan = 750;

        graceTimeLeft = gracePeriodTime;
        initParticleSys(partColor);
        color = partColor;
        
    }

    void initParticleSys(vec3 partColor){
        // cout << color.x <<"\n";
        // thePartSystem = new particleSys(pos, vec3(0.2f, 0.8f, 0.9f));
        // thePartSystem = new particleSys(pos, color);
        thePartSystem = new particleSys(pos, partColor);

        // thePartSystem = new particleSys(pos, vec3(color.x, color.y, color.z));

		thePartSystem->gpuSetup();
    }

    void renderParticleSys(mat4 V, shared_ptr<Program> partProg){
        thePartSystem->setCamera(V);
        thePartSystem->drawMe(partProg);
		thePartSystem->update(pos);
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