//
// Created by Anshuman Komawar on 5/8/22.
//

#ifndef LAB3_MATERIAL_H
#define LAB3_MATERIAL_H
#include <string>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Material{
public:
    int materialId;
    string mtlName;
    vec3 Ka, Kd, Ks, Ke;
    float Ns, Ni, d;

    Material() {
    }

    void setMtlName(string mtlName) {
        this->mtlName = mtlName;
        this->materialId = (int) mtlName.back();
    }

    string getMtlName() {
        return "[" + mtlName + "]";
    }

    void setAttribute(string attribute, string value) {
        if (attribute == "Ns") this->Ns = stof(value);
        else if (attribute == "Ni") this->Ni = stof(value);
        else if (attribute == "d") this->d = stof(value);
        else if (attribute == "Ka") this->Ka = strToVec3(value);
        else if (attribute == "Kd") this->Kd = strToVec3(value);
        else if (attribute == "Ks") this->Ks = strToVec3(value);
        else if (attribute == "Ke") this->Ke = strToVec3(value);
    }

    void getAttributes() {
        cout << "Ns: " << this->Ns << endl;
        cout << "Ni: " << this->Ni << endl;
        cout << "d: " << this->d << endl;
        cout << "Ka: " << this->Ka.x << " " << this->Ka.y << " " << this->Ka.z << endl;
        cout << "Kd: " << this->Kd.x << " " << this->Kd.y << " " << this->Kd.z << endl;
        cout << "Ks: " << this->Ks.x << " " << this->Ks.y << " " << this->Ks.z << endl;
        cout << "Ke: " << this->Ke.x << " " << this->Ke.y << " " << this->Ke.z << endl;
    }


private:
    vec3 strToVec3(string str) {
        return vec3(
            stof(str.substr(0, str.find(" "))),
            stof(str.substr(str.find(" ") + 1, str.rfind(" "))),
            stof(str.substr(str.rfind(" ") + 1))
        );
    }
};


#endif //LAB3_MATERIAL_H
