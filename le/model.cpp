#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"
using namespace std;
#define fuck(x) cout<<x<<endl

Model::Model(const char *filename) : verts_(), faces_(),texture_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            std::vector<int> t;
            int itrash, idx,middle;
            iss >> trash;
            while (iss >> idx >> trash >> middle >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                middle--;
                f.push_back(idx);
                t.push_back(middle);
            }
            faces_.push_back(f);
            texture_.push_back(t);
        }
        else if(!line.compare(0, 2, "vt")){
            iss >> trash;
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            texture_verts_.push_back(v);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::texture(int i)  {
    return texture_[i];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::texture_vert(int i){
    return texture_verts_[i];
}
