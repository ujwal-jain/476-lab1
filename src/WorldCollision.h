//
// Created by Anshuman Komawar on 5/3/22.
// World Collision Model
//

#ifndef LAB3_WORLDCOLLISION_H
#define LAB3_WORLDCOLLISION_H
#include "MatrixStack.h"
#include "map"
#include <iterator>
#include <vector>

using namespace glm;

class WorldCollision {
public:
    class Face {
    public:
        vec3 v1, v2, v3;
        // ax + by + cz + d = 0
        vec4 e1Plane, e2Plane, e3Plane;
        vec3 centerPoint;
        vec3 face;
        float h;

        Face(vec3 v1, vec3 v2, vec3 v3) {
            h = (length(v1) + length(v2) + length(v3)) / 3;

            v1 = normalize(v1);
            v2 = normalize(v2);
            v3 = normalize(v3);

            this->v1= normalize(v1);
            this->v2= normalize(v2);
            this->v3= normalize(v3);

            e1Plane = vec4(cross(v2 - v1, v1), 1);
            e1Plane.w = e1Plane.x * v1.x + e1Plane.y * v1.y + e1Plane.z * v1.z;
            e2Plane = vec4(cross(v3 - v2, v2), 1);
            e2Plane.w = e2Plane.x * v2.x + e2Plane.y * v2.y + e2Plane.z * v2.z;
            e3Plane = vec4(cross(v1 - v3, v3), 1);
            e3Plane.w = e3Plane.x * v3.x + e3Plane.y * v3.y + e1Plane.z * v3.z;

            centerPoint = ((v1 + v2 + v3) / 3.0f);
            if(!testPlane(e1Plane, centerPoint)) {
                e1Plane = vec4(cross(v1, v2 - v1), 1);
                e1Plane.w = e1Plane.x * v1.x + e1Plane.y * v1.y + e1Plane.z * v1.z;
            }
            if(!testPlane(e2Plane, centerPoint)) {
                e2Plane = vec4(cross(v2, v3 - v2), 1);
                e2Plane.w = e2Plane.x * v2.x + e2Plane.y * v2.y + e1Plane.z * v2.z;
            }
            if(!testPlane(e3Plane, centerPoint)) {
                e3Plane = vec4(cross(v3, v1 - v3), 1);
                e3Plane.w = e3Plane.x * v3.x + e3Plane.y * v3.y + e3Plane.z * v3.z;
            }
        }

        bool pointInFace(vec3 point) {
            return testPlane(e1Plane, point) && testPlane(e2Plane, point) && testPlane(e3Plane, point);
        }

        bool testPlane(vec4 plane, vec3 point) {
            return plane.x * point.x + plane.y * point.y + plane.z * point.z > 0;
        }

        void printFace() {
            cout << "v1: " << v1.x << ", " << v1.y << ", " << v1.z << " | ";
            cout << "v2: " << v2.x << ", " << v2.y << ", " << v2.z << " | ";
            cout << "v3: " << v3.x << ", " << v3.y << ", " << v3.z << endl;

            cout << "e1: " << e1Plane.x << ", " << e1Plane.y << ", " << e1Plane.z << ", " << e1Plane.w << endl;
            cout << "e2: " << e2Plane.x << ", " << e2Plane.y << ", " << e2Plane.z << ", " << e2Plane.w << endl;
            cout << "e3: " << e3Plane.x << ", " << e3Plane.y << ", " << e3Plane.z << ", " << e3Plane.w << endl;
        }
    };

    std::vector<Face> faces;

    void getFaces(shared_ptr<Shape> worldOBJ) {
        // identify different faces from eleBuf
        for(int sidx = 0; sidx < worldOBJ->obj_count; sidx++) {
            for (int idx = 0; idx < worldOBJ->eleBuf[sidx].size() / 3; idx++) {
//            for (int idx = 0; idx < 1; idx++) {
                int v1 = worldOBJ->eleBuf[sidx][idx * 3 + 0];
                int v2 = worldOBJ->eleBuf[sidx][idx * 3 + 1];
                int v3 = worldOBJ->eleBuf[sidx][idx * 3 + 2];

                vec3 vert1, vert2, vert3;
                vert1.x = worldOBJ->posBuf[sidx][v1 * 3 + 0];
                vert1.y = worldOBJ->posBuf[sidx][v1 * 3 + 1];
                vert1.z = worldOBJ->posBuf[sidx][v1 * 3 + 2];
                vert2.x = worldOBJ->posBuf[sidx][v2 * 3 + 0];
                vert2.y = worldOBJ->posBuf[sidx][v2 * 3 + 1];
                vert2.z = worldOBJ->posBuf[sidx][v2 * 3 + 2];
                vert3.x = worldOBJ->posBuf[sidx][v3 * 3 + 0];
                vert3.y = worldOBJ->posBuf[sidx][v3 * 3 + 1];
                vert3.z = worldOBJ->posBuf[sidx][v3 * 3 + 2];
                Face face = Face(vert1, vert2, vert3);
                faces.emplace_back(face);
            }
        }
    }

    // returns vec3(0, 0, 0) if valid
    // returns possible new velocity if not valid
    bool isLocationValid(vec3 pos, float height) {
        return height >= getFaceHeight(pos);
    }

    vec3 reflectVelocity(vec4 plane, vec3 d) {
        vec3 n = -vec3(plane);
        n = normalize(n);
        return d - dot(2.f * d, n) * n;
    }

    // return vec3(0, 0, 0) is location is valid
    // otherwise return the new rotation axis of the projectile
    vec3 isProjLocationValid(vec3 oldPos, vec3 newPos, float height) {
        int faceIdx = getMaxHeightFace(newPos);
        Face face = faces[faceIdx];
        if(height >= face.h)
            return vec3(0, 0, 0);
        else {
            vec3 velDir = newPos - oldPos;
            if(!face.testPlane(face.e1Plane, oldPos)) {
                return normalize(cross(oldPos, reflectVelocity(face.e1Plane, velDir)));
            }
            else if(!face.testPlane(face.e2Plane, oldPos)) {
                return normalize(cross(oldPos, reflectVelocity(face.e2Plane, velDir)));
            }
            else {
                return normalize(cross(oldPos, reflectVelocity(face.e3Plane, velDir)));
            }
        }

    }

    float getFaceHeight(vec3 pos) {
        float maxHeight = 0;
        for(Face face: faces) {
            if (face.h < 0.5 && face.pointInFace(pos)) {
                maxHeight = max({maxHeight, face.h});
            }
        }
        return maxHeight;
    }

    int getMaxHeightFace(vec3 pos) {
        float maxHeight = 0;
        int maxHeightIdx = 0;
        for(int i = 0; i < faces.size(); i++) {
            Face face = faces[i];
            if (face.h < 0.5 && face.pointInFace(pos)) {
                if(face.h > maxHeight) {
                    maxHeightIdx = i;
                    maxHeight = face.h;
                }
            }
        }
        return maxHeightIdx;
    }

    void printVec(vec3 v) {
        cout << "v: " << v.x << ", " << v.y << ", " << v.z << endl;
    }
};
#endif