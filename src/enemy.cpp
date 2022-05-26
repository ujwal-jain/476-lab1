#include <iostream>

#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"
using namespace std;
using namespace glm;

#define PLAYERSCALE 1.0f
#define ROTATIONSPEED 2.0f
#define PROJSPAWN 100
#define PI 3.14f

#ifndef PROJECTILE
#include "projectile.cpp"
#endif

class Enemy
{
public:
    int w, a, s, d, space;
    vec3 pos;
    mat4 rotation;
    mat4 projectileRot;
    vec3 fwd, right, up;
    int spawnTime;
    boundingsphere hitbox;

    Enemy(vec3 forward) {
//        forward = vec3(1, 0, 0);
        pos = 15.f * forward;

        // find rotation matrix that changes 0, 0, -1 -> fwd
        vec3 vb = vec3(0, 0, -1);
        vec3 va = normalize(forward);
        rotation = mat4(1);
        vec3 v = cross(va, vb);
        float sinv = length(v);
        float cosv = dot(va, vb);

        // R = I + [v]x + [v]x^2((1 - c) / s^2)
        // [v]x is the skew symmetric cross product matrix of v
        mat3 vx = mat3(0);
        vx[0][1] = -v[2];
        vx[0][2] = v[1];
        vx[1][0] = v[2];
        vx[1][2] = -v[0];
        vx[2][0] = -v[1];
        vx[2][1] = v[0];

        mat3 R = mat3(1) + vx + (vx*vx)*(1 - cosv) / (sinv*sinv);
        rotation = mat4(R);

        projectileRot = mat4(1);
        w = a = s = d = space = 0;

        fwd = forward;
        up = cross(fwd, vec3(0, 1, 0));
        right = cross(fwd, up);

        spawnTime = PROJSPAWN;
        hitbox = boundingsphere(pos, 0.5f);
    }

    mat4 getModel() const {
        // player rotation around world
        return rotation
               * rotate(mat4(1), PI / 4, vec3(0, 0, 1))
               // height above world
               * translate(mat4(1), vec3(0, 0, -14))
               // player rotation about mouse
               * projectileRot
               // rotation to orient the player correctly
               * rotate(mat4(1), -PI / 2, vec3(1, 0, 0))
               * rotate(mat4(1), -PI * 2, vec3(0, 1, 0))
               // scale the player
               * scale(mat4(1), vec3(PLAYERSCALE));
    }

    bool updateEnemy(vec3 playerFwd, float ftime) {

        vec4 newDir4 = vec4(up, 1) * rotate(mat4(1), ROTATIONSPEED * ftime, fwd);
        up = normalize(vec3(newDir4));

        right = cross(fwd, up);
        projectileRot = projectileRot * rotate(mat4(1), ROTATIONSPEED * ftime, vec3(0, 0, 1));
        // rotation = rotation * rotate(mat4(1), ROTATIONSPEED * ftime, vec3(0, 0, 1));

        if(spawnTime == 0) {
            spawnTime = rand() % PROJSPAWN + PROJSPAWN / 2;
            return true;
        }
        else {
            spawnTime--;
            return false;
        }
    }

    Projectile spawnProjectile() {
        return Projectile(pos,-up, 0, vec3(0.9f, 0.3f, 0.1f));
//        return Projectile(pos, vec3(pDir.x, pDir.y, pDir.z));
    }
};