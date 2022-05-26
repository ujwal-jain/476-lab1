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
    int gridSize;
    std::vector<vector<float>> grid;
    std::vector<vector<bool>> gridFilled;

    float globalMinX = 0;
    float globalMinY = 0;
    float globalMaxX = 0;
    float globalMaxY = 0;

    class Face {
    public:
        vec3 v1, v2, v3;
        // ax + by + cz + d = 0
        vec4 e1Plane, e2Plane, e3Plane;
        vec3 centerPoint;
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
    };

    std::vector<Face> triFaces;

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
                triFaces.emplace_back(vert1, vert2, vert3);
            }
        }
    }

    void testFace() {
//        vec3 v1 = vec3(0.2, -0.3, -1);
//        vec3 v3 = vec3(-0.2, -0.3, -1);
//        vec3 v2 = vec3(0, 0.3, -1);
//
//        Face face = Face(v1, v2, v3);
//        printFace(face);
//        cout << face.pointInFace(vec3(0, 0, 13)) << endl;
    }

    bool nextLocationValid(vec3 pos, float height) {
        float mHeight = 0;
        for(Face face: triFaces) {
            if (face.h < 0.5 && face.pointInFace(pos)) {
                mHeight = max({mHeight, face.h});
            }
        }
        return height >= mHeight;

    }

    void printFace(Face face) {
        cout << "v1: " << face.v1.x << ", " << face.v1.y << ", " << face.v1.z << " | ";
        cout << "v2: " << face.v2.x << ", " << face.v2.y << ", " << face.v2.z << " | ";
        cout << "v3: " << face.v3.x << ", " << face.v3.y << ", " << face.v3.z << endl;

        cout << "e1: " << face.e1Plane.x << ", " << face.e1Plane.y << ", " << face.e1Plane.z << ", " << face.e1Plane.w << endl;
        cout << "e2: " << face.e2Plane.x << ", " << face.e2Plane.y << ", " << face.e2Plane.z << ", " << face.e2Plane.w << endl;
        cout << "e3: " << face.e3Plane.x << ", " << face.e3Plane.y << ", " << face.e3Plane.z << ", " << face.e3Plane.w << endl;
    }

    void printVec(vec3 v) {
        cout << "v: " << v.x << ", " << v.y << ", " << v.z << endl;
    }

    /* Create a 3d grid representing faces in the 3d Object
     * input: shared pointer -> Shape
     * output: N/A
    */
    void get3dGrid(shared_ptr<Shape> worldOBJ) {
        // init grid size
        gridSize = 10;

        // identify different faces from eleBuf
        for (int idx = 0; idx < worldOBJ->eleBuf[0].size() / 3; idx++) {
            int v1 = worldOBJ->eleBuf[0][idx * 3 + 0];
            int v2 = worldOBJ->eleBuf[0][idx * 3 + 1];
            int v3 = worldOBJ->eleBuf[0][idx * 3 + 2];

            faces.push_back(make_tuple(convertTo2D(worldOBJ, v1), convertTo2D(worldOBJ, v2), convertTo2D(worldOBJ, v3)));
        }

        // init grid with NULL values
        for (int idx = 0; idx < gridSize; idx++) {
            vector<float> temp;
            vector<bool> tempFilled;
            for (int idx2 = 0; idx2 < gridSize; idx2++) {
                temp.push_back(0.0f);
                tempFilled.push_back(false);
            }
            grid.push_back(temp);
            gridFilled.push_back(tempFilled);
        }


        // rasterization of faces
        for (tuple<Vertex2D, Vertex2D, Vertex2D> face: faces) {
            Vertex2D v1 = get<0>(face);
            Vertex2D v2 = get<1>(face);
            Vertex2D v3 = get<2>(face);

            float r = max({v1.r, v2.r, v3.r});
            float minx, maxx, miny, maxy;

            minx = min({v1.i, v2.i, v3.i});
            maxx = max({v1.i, v2.i, v3.i});
            miny = min({v1.j, v2.j, v3.j});
            maxy = max({v1.j, v2.j, v3.j});

            v1.i = mapToRange(v1.i, -1, 1, 0, gridSize - 1);
            v1.j = mapToRange(v1.j, -1, 1, 0, gridSize - 1);

            v2.i = mapToRange(v2.i, -1, 1, 0, gridSize - 1);
            v2.j = mapToRange(v2.j, -1, 1, 0, gridSize - 1);

            v3.i = mapToRange(v3.i, -1, 1, 0, gridSize - 1);
            v3.j = mapToRange(v3.j, -1, 1, 0, gridSize - 1);

            minx = min({v1.i, v2.i, v3.i});
            maxx = max({v1.i, v2.i, v3.i});
            miny = min({v1.j, v2.j, v3.j});
            maxy = max({v1.j, v2.j, v3.j});

            for (int x = minx; x <= maxx; x++) {
                for (int y = miny; y <= maxy; y++) {
                    if(PointInTriangle(Vertex2D(0, x, y), v1, v2, v3)) {
                        grid[x][y] = max({grid[x][y], r});
                        gridFilled[x][y] = true;
                    }
                }
            }
        }
    }

    bool didPlayerCollide(shared_ptr<Shape> worldOBJ, vec3 playerPos, float playerHeight) {
        float r = length(vec3(playerPos.x, playerPos.y, playerPos.z));
        float t = acos(playerPos.z / r);
        float p = playerPos.x != 0 && playerPos.y != 0 ? atan(playerPos.y / playerPos.x) : 0;

        t = round(mapToRange(t, 0, 3.14159, 0, gridSize - 1));
        p = round(mapToRange(p, -1.5708, 1.57078, 0, gridSize - 1));


//        printf("%f %f %d\n", playerHeight, grid[t][p], gridFilled[t][p]);
//        cout << playerHeight << " " << grid[t][p] << " " << gridFilled[t][p] << endl;
        if (playerHeight <= grid[t][p]) return true;
        return false;
    }

    float getHeight(vec3 playerPos) {
        float r = length(vec3(playerPos.x, playerPos.y, playerPos.z));
        float t = acos(playerPos.z / r);
        float p = playerPos.x != 0 && playerPos.y != 0 ? atan(playerPos.y / playerPos.x) : 0;

        int x = round(mapToRange(t, 0, 3.14159, 0, gridSize - 1));
        int y = round(mapToRange(p, -1.5708, 1.57078, 0, gridSize - 1));
//        printf("%d %d %d\n", gridSize, x, y);
        return grid[x][y];
    }

private:

    class WorldVertex {
    public:
        float x, y, z;

        WorldVertex(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        WorldVertex(vec3 v) {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
        }
    };

    std::vector<tuple<WorldVertex, WorldVertex, WorldVertex>> faces3D;

    class Vertex2D {
    public:
        float i;
        float j;
        float r;

        Vertex2D(float r, float i, float j) {
            this->r = r;
            this->i = i;
            this->j = j;
        }
    };

    std::vector<tuple<Vertex2D, Vertex2D, Vertex2D>> faces;

    float sign (Vertex2D p1, Vertex2D p2, Vertex2D p3)
    {
        return (p1.i - p3.i) * (p2.j - p3.j) - (p2.i - p3.i) * (p1.j - p3.j);
    }

    bool PointInTriangle (Vertex2D pt, Vertex2D v1, Vertex2D v2, Vertex2D v3)
    {
        float d1, d2, d3;
        bool has_neg, has_pos;

        d1 = sign(pt, v1, v2);
        d2 = sign(pt, v2, v3);
        d3 = sign(pt, v3, v1);

        has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(has_neg && has_pos);
    }


    Vertex2D convertTo2D(shared_ptr<Shape> worldOBJ, int idx) {
        float x = worldOBJ->posBuf[0][idx * 3 + 0];
        float y = worldOBJ->posBuf[0][idx * 3 + 1];
        float z = worldOBJ->posBuf[0][idx * 3 + 2];
        float r = length(vec3(x, y, z));
        float t = acos(z / r);
        float p = atan(y / x);

//        cout << "x: " << x << " y: " << y << " z: " << z << endl;
//        cout << "r: " << r << endl;

        return Vertex2D(r, mapToRange(t, 0, 3.14159, -1, 1), mapToRange(p, -1.5708, 1.57078, -1, 1));
    }

    float mapToRange(float oldValue, float oldMin, float oldMax, float newMin, float newMax) {
        float OldRange = (oldMax - oldMin);
        float NewRange = (newMax - (newMin));
        return (((oldValue - oldMin) * NewRange) / OldRange) + (newMin);
    }
};


#endif