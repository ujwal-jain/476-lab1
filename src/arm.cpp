#include <iostream>

#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"
using namespace std;
using namespace glm;

#define ARMSCALE 0.7f
#define PI 3.14159265f

#ifndef PROJECTILE
#include "projectile.cpp"
#endif

class Arm
{
public:
    int w, a, s, d, space;
    vec3 pos;
    vec3 mouseDir;
    mat4 rotation;
    mat4 fireRotation;
    mat4 mouseRot;
    vec3 fwd, right, up;
    bool armRot;
    float totalRot;
    float lastRot;

    Arm()
    {
        pos = vec3(0, 0, 15);
        rotation = mat4(1);
        fireRotation = mat4(1);
        mouseRot = mat4(1);
        w = a = s = d = space = 0;
        fwd = vec3(0, 0, -1);
        up = vec3(0, 1, 0);
        right = cross(fwd, up);
        armRot = false;
        totalRot = 0;
        lastRot = 0;
    }

    mat4 getModel() const {
        // player rotation around world
        return rotation
                // height above world
                * translate(mat4(1), vec3(0, 0, -13))
                // player rotation about mouse
                * mouseRot
                // rotation to orient the player correctly
                * rotate(mat4(1), PI / 2, vec3(1, 0, 0))

                * rotate(mat4(1), 0.7f + PI / 2 , vec3(0, 1, 0))

                * fireRotation

                * translate(mat4(1), vec3(-0.3, -0.1, 0))

                //* rotate(mat4(1), PI / 2, vec3(1, 0, 0))
                // scale the player
                * scale(mat4(1), vec3(ARMSCALE));
    }

    void updateLocation(float dt)
    {
        // either about right, up, right + up, or right - up given a combination of wasd
        if(w) {
            if(d) {
                rotatePlayer(-dt, normalize(right + up));
            }
            else if(a) {
                rotatePlayer(-dt, normalize(right - up));
            }
            else {
                rotatePlayer(-dt, right);
            }
        }
        else if(s) {
            if(d) {
                rotatePlayer(dt, normalize(right - up));
            }
            else if(a) {
                rotatePlayer(dt, normalize(right + up));
            }
            else {
                rotatePlayer(dt, right);
            }
        }
        else if(d){
            rotatePlayer(-dt, up);
        }
        else if(a){
            rotatePlayer(dt, up);
        }

        if(space){
            armRot = true;
            space = 0;
        }
        if(armRot){
            twistArm();
        }
        
        pos = 15.f * fwd;
    }


    void twistArm(){
        fireRotation *= rotate(mat4(1), 0.3f, vec3(1, 0, 0));
        totalRot += abs(lastRot - fireRotation[1][1]);
        lastRot = fireRotation[1][1];
        if(totalRot > 3.9f){
            fireRotation = mat4(1);
            totalRot = 0;
            armRot = false;
        }
    }

    void armRotation(GLFWwindow *window, double xpos, double ypos) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float xposFloat = static_cast<float>(xpos);
        float yposFloat = static_cast<float>(ypos);

        float mouseDirX = -(xposFloat - (width / 2));
        float mouseDirY = -(yposFloat - (height / 2));
        // direction of the mouse relative to the screen, that is why z is always 0.
        mouseDir = vec3(normalize(vec2(mouseDirX, mouseDirY)), 0);
        // rotation of the player towards the mouse direction
        mouseRot = rotate(mat4(1), atan(mouseDir.y, mouseDir.x) - PI/4.0f, vec3(0, 0, 1));
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
        vec3 pDir = normalize(pDirRight + pDirUp);\
        //cout << fwd.x;
        return Projectile(pos + cross(fwd, pDir), vec3(pDir.x, pDir.y, pDir.z), 10);
    }

private:
    // rotates the player an angle around a given world axis and updates fwd, up, right vectors
    void rotatePlayer(float angle, vec3 axis) {
        mat4 R = rotate(mat4(1), angle, axis);
        mat4 Ri = rotate(mat4(1), -angle, axis);
        vec4 fwd4 = vec4(fwd, 1) * R;
        fwd = normalize(vec3(fwd4));
        vec4 up4 = vec4(up, 1) * R;
        up = normalize(vec3(up4));
        right = cross(fwd, up);
        rotation = Ri * rotation;
    }
};