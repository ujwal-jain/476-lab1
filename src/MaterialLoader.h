//
// Created by Anshuman Komawar on 5/8/22.
//

#ifndef LAB3_MATERIALLOADER_H
#define LAB3_MATERIALLOADER_H
#include <string>
#include "vector"
#include "iostream"
#include "fstream"
#include "sstream"
#include "Material.h"

using namespace std;

class MaterialLoader{
public:
//    map<int, Material> materials;
    vector<Material> materials;
    MaterialLoader(string filename) {
        this->filename = filename;
        file = ifstream(filename);
    };

    void readMaterialFile() {
        // if file doesn't exist
        if (!file) {
            cout << "Unable to open file: " << filename << endl;
            return;
        }

        cout << "Reading material file: " << filename << endl;

        // if file exists read file contents into buffer
        std::string buffer((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

        // parse buffer
        unsigned long mtl = buffer.find("newmtl");
        unsigned long end = buffer.find("\n\n", mtl);
        while (end != buffer.npos) {
            // create material type
            Material material;

            // get line
            string line = buffer.substr(mtl, end - mtl);

            // get material name
            unsigned long mtlName = line.find(" ") + 1;
            unsigned long endline = line.find("\n", mtlName);

            material.setMtlName(line.substr(mtlName, endline - mtlName));

            // get material properties
            unsigned long pos = 0;
            while ((pos = line.find("\n", endline + 1)) <= end) {
                unsigned long attributeName = line.find(" ", endline + 1);
                material.setAttribute(line.substr(endline + 1, attributeName - endline - 1), line.substr(attributeName + 1, pos - attributeName - 1));
                endline = pos;
            }

            // add material to list
            materials.push_back(material);

            mtl = buffer.find("newmtl", end);
            end = buffer.find("\n\n", mtl);
        }
    }

private:
    string filename;
    ifstream file;
};

#endif //LAB3_MATERIALLOADER_H
