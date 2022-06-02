#include <iostream>

#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"
using namespace std;
using namespace glm;

#define PLAYERSCALE 0.7f
#define PLAYERROTSPEED 0.6f
#define PI 3.14f
#define PLAYERPOSHEIGHT 13.6f

#ifndef PROJECTILE
#include "projectile.cpp"
#endif

class Player
{
public:
    int w, a, s, d, space;
    vec3 pos;
    vec3 mouseDir;
    mat4 rotation;
    mat4 mouseRot;
    vec3 fwd, right, up;
    float height;
    int health;
    boundingsphere hitbox;
    float angle;
    vec3 axis;
    particleSys *thePartSystem;
    vec3 deathColor;


    Player()
    {
        pos = vec3(0, 0, PLAYERPOSHEIGHT);
        rotation = mat4(1);
        mouseRot = mat4(1);
        w = a = s = d = space = 0;
        fwd = vec3(0, 0, -1);
        up = vec3(0, 1, 0);
        right = cross(fwd, up);
        height = 0.43f;
        hitbox = boundingsphere(pos, 1.f);
        health = 10;
        vec3 deathColor = vec3(0.9f, 0.8f, 0.7f);
        
    }

    mat4 getModel() const {
        // player rotation around world
        return rotation
                * rotate(mat4(1), PI / 4, vec3(0, 0, 1))
                // height above world
                * translate(mat4(1), vec3(0, 0, -1 * PLAYERPOSHEIGHT))
                // player rotation about mouse
                * mouseRot
                // rotation to orient the player correctly
                * rotate(mat4(1), -PI / 2, vec3(1, 0, 0))
                // scale the player
                * translate(mat4(1), vec3(0.2f, 0, 0))
                * scale(mat4(1), vec3(PLAYERSCALE));
    }

    mat4 getModelHealth() const {
        // player rotation around world
        return rotation
               // height above world
               * translate(mat4(1), vec3(0, 0, -1 * (PLAYERPOSHEIGHT + 2)))
                 * rotate(mat4(1), PI / 4, vec3(0, 0, 1))
               // player rotation about mouse
               * rotate(mat4(1), PI / 4, vec3(0, 0, 1))
               //
                 * rotate(mat4(1), PI / 4, vec3(0, 1, 0))
               // rotation to orient the player correctly
               * rotate(mat4(1), -PI / 2, vec3(1, 0, 0))
               // scale the player
               * scale(mat4(1), vec3(PLAYERSCALE, PLAYERSCALE, health * PLAYERSCALE * 0.125));
    }


    void updateLocation()
    {
        if(angle != 0 && axis != vec3(0, 0, 0))
            rotatePlayer(angle, axis);
        pos = PLAYERPOSHEIGHT * fwd;
        hitbox.center = pos;
    }

    mat4 getModelAim() const {
        // player rotation around world
        return rotation
               * rotate(mat4(1), PI / 4, vec3(0, 0, 1))
               // height above world
               * translate(mat4(1), vec3(0, 0, -1 * PLAYERPOSHEIGHT))
               // player rotation about mouse
               * mouseRot
               // rotation to orient the player correctly
               * rotate(mat4(1), PI / 2, vec3(1, 0, 0))
               * translate(mat4(1), vec3(0, 0.5, -1))
               // scale the player
               * scale(mat4(1), vec3(PLAYERSCALE));
    }
    vec3 getNextLocation(float dt)
    {
        // either about right, up, right + up, or right - up given a combination of wasd
        angle = 0;
        axis = vec3(0, 0, 0);
        if(w) {
            if(d) {
                angle = PLAYERROTSPEED * -dt;
                axis = normalize(right + up);
            }
            else if(a) {
                angle = PLAYERROTSPEED * -dt;
                axis = normalize(right - up);
            }
            else {
                angle = PLAYERROTSPEED * -dt;
                axis = normalize(right);
            }
        }
        else if(s) {
            if(d) {
                angle = PLAYERROTSPEED * dt;
                axis = normalize(right - up);
            }
            else if(a) {
                angle = PLAYERROTSPEED * dt;
                axis = normalize(right + up);
            }
            else {
                angle = PLAYERROTSPEED * dt;
                axis = normalize(right);
            }
        }
        else if(d) {
            angle = PLAYERROTSPEED * -dt;
            axis = normalize(up);
        }
        else if(a) {
            angle = PLAYERROTSPEED * dt;
            axis = normalize(up);
        }
        if(angle == 0 && axis == vec3(0, 0, 0)) {
            return pos;
        }
        else {
            vec4 fwd4 = vec4(fwd, 1) * rotate(mat4(1), angle, axis);
            vec3 fwd3 = normalize(vec3(fwd4));
            return PLAYERPOSHEIGHT * fwd3;
        }
    }

    void playerRotation(GLFWwindow *window, double xpos, double ypos) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float xposFloat = static_cast<float>(xpos);
        float yposFloat = static_cast<float>(ypos);

        float mouseDirX = -(xposFloat - (width / 2));
        float mouseDirY = -(yposFloat - (height / 2));
        // direction of the mouse relative to the screen, that is why z is always 0.
        mouseDir = vec3(normalize(vec2(mouseDirX, mouseDirY)), 0);
        // rotation of the player towards the mouse direction
        mouseRot = rotate(mat4(1), atan(mouseDir.y, mouseDir.x) + PI + PI/4.0f, vec3(0, 0, 1));
    }

    Projectile spawnProjectile() {
        // pDirRight represents the x component of the mouseDir vector in world space
        float mouseRightToUp = abs(mouseDir.x / mouseDir.y);
        vec3 pDirRight = right * mouseRightToUp;
        if(mouseDir.x < 0)
            pDirRight = -pDirRight;

        // pDirUp represents the y component of the mouseDir vector in world space
        float mouseUpToRight = abs(mouseDir.y / mouseDir.x);
        vec3 pDirUp = up * mouseUpToRight;
        if(mouseDir.y < 0)
            pDirUp = -pDirUp;

        // pDir represents the direction of the projectile derived from the mouseDir
        vec3 pDir = normalize(pDirRight + pDirUp);
        return Projectile(pos, vec3(pDir.x, pDir.y, pDir.z) * 1.5f, 10, vec3(0.2f, 0.8f, 0.9f));
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
    void playerDeath(){
        initParticleSys(deathColor);
    }

private:

    // rotates the player an angle around a given world axis and updates fwd, up, right vectors
    mat4 rotatePlayer(float t, vec3 a) {
        mat4 R = rotate(mat4(1), t, a);
        mat4 Ri = rotate(mat4(1), -t, a);
        vec4 fwd4 = vec4(fwd, 1) * R;
        fwd = normalize(vec3(fwd4));
        vec4 up4 = vec4(up, 1) * R;
        up = normalize(vec3(up4));
        right = cross(fwd, up);
        rotation = Ri * rotation;
        return R;
    }
};