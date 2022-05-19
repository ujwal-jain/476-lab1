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
    std::vector<vector<float>> grid;

    /* Create a 3d grid representing faces in the 3d Object
     * input: shared pointer -> Shape
     * output: N/A
    */
    void get3dGrid(shared_ptr<Shape> worldOBJ) {
        // init grid size
        int gridSize = (int)sqrt(worldOBJ->posBuf[0].size() / 3);

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
            for (int idx2 = 0; idx2 < gridSize; idx2++) {
                temp.push_back(0.0f);
            }
            grid.push_back(temp);
        }

        // rasterization of faces
        for (tuple<Vertex2D, Vertex2D, Vertex2D> face: faces) {
            Vertex2D v1 = get<0>(face);
            Vertex2D v2 = get<1>(face);
            Vertex2D v3 = get<2>(face);
            float r = (v1.r + v2.r + v3.r) / 3;
            float minx, maxx, miny, maxy;

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

            for (int x = minx; x < maxx; x++) {
                for (int y = miny; y < maxy; y++) {
                    if(PointInTriangle(Vertex2D(0, x, y), v1, v2, v3)) grid[x][y] = r;
                }
            }
        }
    }

    bool didPlayerCollide(shared_ptr<Shape> worldOBJ, vec3 playerPos, float playerHeight) {
        int gridSize = (int)sqrt(worldOBJ->posBuf[0].size() / 3);
        float r = length(vec3(playerPos.x, playerPos.y, playerPos.z));
        float t = acos(playerPos.z / r);
        float p = playerPos.x != 0 && playerPos.y != 0 ? atan(playerPos.y / playerPos.x) : 0;

        t = (int)mapToRange(t, 0, 3.14159, 0, gridSize - 1);
        p = (int)mapToRange(p, -1.5708, 1.57078, 0, gridSize - 1);

        printf("%f %f\n", playerHeight, grid[t][p]);
        if (playerHeight <= grid[t][p]) return true;
        return false;
    }

private:

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

        return Vertex2D(r, mapToRange(t, 0, 3.14159, -1, 1), mapToRange(p, -1.5708, 1.57078, -1, 1));
    }

    float mapToRange(float oldValue, float oldMin, float oldMax, float newMin, float newMax) {
        float OldRange = (oldMax - oldMin);
        float NewRange = (newMax - (newMin));
        return (((oldValue - oldMin) * NewRange) / OldRange) + (newMin);
    }
};
#endif