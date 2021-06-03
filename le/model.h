//
// Created by 10 on 2021/3/25.
//

#ifndef LE_MODEL_H
#define LE_MODEL_H
#include <vector>
#include "geometry.h"

class Model{
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<int> >  faces_;
public:
    Model(const char*filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    std::vector<int> face(int idx);
};


#endif //LE_MODEL_H
