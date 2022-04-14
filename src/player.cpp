#include "MatrixStack.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "boundingsphere.h"

class Player
{
public:
    glm::vec3 pos, rot;
    int w, a, s, d;
    glm::vec3 direction;
    bool firstMouse = true;
    float yaw2, pitch2, lastX, lastY;
    glm::vec3 cameraPos, cameraFront, cameraUp;
    float camX, camY, camZ;
    boundingsphere sphere;
    float speed;

    Player()
    {
        w = a = s = d = 0;
        pos = rot = glm::vec3(0, 0, 0);
        yaw2   = -90.0f;	// y
        pitch2 =  0.0f;
        lastX =  1920.0f / 2.0;
        lastY =  1080.0 / 2.0;
        camX = camY = camZ = 0.0f;
        sphere = boundingsphere(pos, 1.f);
        speed = 0.075;
    }
    glm::mat4 process(double ftime)
    {

        glm::vec3 horizontal = cross(cameraFront, glm::vec3(0, 1, 0));
        if (w) {
            camX += speed * cameraFront.x;
            camZ += speed * cameraFront.z;
        }

        if (s) {
            camX -= speed * cameraFront.x;
            camZ -= speed * cameraFront.z;
        }

        if (a) {
            camZ -= speed * horizontal.z;
            camX -= speed * horizontal.x;
        }

        if (d) {
            camZ += speed * horizontal.z;
            camX += speed * horizontal.x;
        }

        cameraPos = glm::vec3(camX, camY, camZ);
        sphere.updateCenter(cameraPos);
        return glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0, 1, 0));
    }

    void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
        float xposFloat = static_cast<float>(xpos);
        float yposFloat = static_cast<float>(ypos);

        if (firstMouse)
        {
            lastX = xposFloat;
            lastY = yposFloat;
            firstMouse = false;
        }

        float xoffset = xposFloat - lastX;
        float yoffset = lastY - yposFloat; // reversed since y-coordinates go from bottom to top
        lastX = xpos; // old
        lastY = ypos; // old

        float sensitivity = 0.1f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw2 += xoffset;
        pitch2 += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch2 > 89.0f)
            pitch2 = 89.0f;
        if (pitch2 < -89.0f)
            pitch2 = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw2)) * cos(glm::radians(pitch2));
        front.y = sin(glm::radians(pitch2));
        front.z = sin(glm::radians(yaw2)) * cos(glm::radians(pitch2));
        cameraFront = glm::normalize(front);
    };

    void increaseSpeed() {
        speed += 0.01;
    }

    void decreaseSpeed() {
        if (speed > 0.01) {
            speed -= 0.01;
        }
    }
};