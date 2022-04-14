#ifndef COLLISIONS_H
#define COLLISIONS_H
#include "MatrixStack.h"
#include "boundingplane.h"
#include "boundingsphere.h"

using namespace glm;

class collisions {
public:
    static bool detectPlaneSphere(boundingplane plane, boundingsphere sphere) {
        vec3 planeNormal = plane.getNormal();
        vec3 sphereCenter = sphere.center;
        float sphereRadius = sphere.radius;
        float distance = dot(planeNormal, sphereCenter) - plane.getD();
        return distance <= sphereRadius;
    }

    static bool detectSphereSphere(boundingsphere sphere1, boundingsphere sphere2) {
        vec3 sphere1Center = sphere1.center;
        vec3 sphere2Center = sphere2.center;
        float sphere1Radius = sphere1.radius;
        float sphere2Radius = sphere2.radius;
        float distance = length(sphere1Center - sphere2Center);
        return distance <= sphere1Radius + sphere2Radius;
    }
};
#endif