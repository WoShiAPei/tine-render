#include <bits/stdc++.h>
#include "tgaimage.h"
#include "model.h"
using namespace std;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,255,0,255);
const TGAColor blue = TGAColor(0,0,255,255);
Model* model = NULL;
const int width = 800;
const int height = 800;

void line1(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color){
    for(float t = 0.; t<1.; t+=.01){
        int x = x0 + (x1-x0)*t;
        int y = y0 + (y1-y0)*t;
        image.set(x,y,color);
    }
}

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color){
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)){//没有洞，画得好看
        std::swap(x0,y0);
        std::swap(x1,y1);
        steep = true;
    }
    if(x0>x1){
        std::swap(x0,x1);
        std::swap(y0,y1);
    }
    for(int x = x0; x<=x1; x++){
        float t = (x-x0) / (float)(x1-x0);
        int y = y0*(1.-t) + y1*t;
        if(steep){
            image.set(y,x,color);
        }
        else{
            image.set(x,y,color);
        }
    }
}
void line(Vec2i t0,Vec2i t1,TGAImage& image, TGAColor color){
    line(t0.x,t0.y,t1.x,t1.y,image,color);
}

void sweepLine(Vec2i t0,Vec2i t1,Vec2i t2,TGAImage& image, TGAColor color){//扫描线画三角形，两个part
    int total_height = t2.y - t0.y;
    int segement_height = t1.y - t0.y;
    bool secondPart = false;
    for(int i = 0;i<total_height;i++){
        if(i>segement_height||i == segement_height){
            secondPart = true;
            segement_height = t2.y - t1.y;
        }
        int y = t0.y+i;
        float alpha = (float)(y - t0.y) /total_height,beta;
        Vec2i A,B;
        if(secondPart){
            beta = (float)(y - t1.y)/segement_height;
            B = t1 + (t2-t1)*beta;
        }
        else{
            beta = (float)(y - t0.y) / segement_height;
            B = t0 + (t1-t0)*beta;
        }
        A = t0 + (t2-t0)*alpha;
        if(A.x>B.x)swap(A,B);
        for(int i = A.x;i<=B.x;i++){
            image.set(i,y,color);
        }
    }
}
Vec3f cross(Vec3f A,Vec3f B){
    return Vec3f(A.y*B.z - B.y*A.z,-(A.x*B.z-B.x*A.z),A.x*B.y-B.x*A.y);
}

Vec3f Compute(Vec2i* pts,Vec2i p){
    Vec3f uv = cross(Vec3f(pts[2].x - pts[0].x,pts[1].x - pts[0].x,  pts[0].x - p.x),Vec3f(pts[2].y - pts[0].y,pts[1].y - pts[0].y,  pts[0].y - p.y));
    if(abs(uv.z)<1)return Vec3f(-1,1,1);
    return Vec3f(1.f - (uv.x+uv.y)/uv.z,uv.x/uv.z,uv.y/uv.z);
}

void barycentric(Vec2i* pts,TGAImage& image, TGAColor color){//插值，包围框内选点
    Vec2i MinBox(image.get_width()-1,image.get_height()-1);
    Vec2i MaxBox(0,0);
    for(int i = 0;i<3;i++){
        MinBox.x = max(0,min(MinBox.x,pts[i].x));
        MinBox.y = max(0,min(MinBox.y,pts[i].y));
        MaxBox.x = min(image.get_width()-1,max(MaxBox.x,pts[i].x));
        MaxBox.y = min(image.get_height()-1,max(MaxBox.y,pts[i].y));
    }
    for(int i = MinBox.x;i<=MaxBox.x;i++){
        for(int j = MinBox.y;j<=MaxBox.y;j++){
            Vec3f temp = Compute(pts,Vec2i(i,j));
            if(temp.x<0||temp.y<0||temp.z<0)continue;
            image.set(i,j,color);
        }
    }
}

void triangle(Vec2i t0,Vec2i t1,Vec2i t2,TGAImage& image, TGAColor color){
    //sweepLine(t0,t1,t2,image,color);
    Vec2i pts[3] = {t0,t1,t2};
    barycentric(pts,image,color);
}

void rasterize(Vec2i p0,Vec2i p1,TGAImage &image,TGAColor color,int ybuffer[]){//ybuffer存y这一维的可见值？
    if(p0.x>p1.x){//保证x维有序
        std::swap(p0,p1);
    }
    for(int x = p0.x;x<=p1.x;x++){
        float t = (x - p0.x)/(float )(p1.x - p0.x);
        int y = p0.y*(1.-t) + p1.y*t;
        if(ybuffer[x]<y){
            ybuffer[x] = y;
            image.set(x,0,color);
        }
    }
}

int main() {
    {
        TGAImage scene(width, height, TGAImage::RGB);
        line(Vec2i(20, 34), Vec2i(744, 400), scene, red);
        line(Vec2i(120, 434), Vec2i(444, 400), scene, green);
        line(Vec2i(330, 463), Vec2i(594, 200), scene, blue);

        line(Vec2i(10, 10), Vec2i(790, 10), scene, white);

        scene.flip_vertically();
        scene.write_tga_file("scene.tga");
    }

    {
        TGAImage render(width,16,TGAImage::RGB);
        int ybuffer[width];
        for(int i = 0;i<width;i++){
            ybuffer[i] = std::numeric_limits<int>::min();//2^-31
        }
        rasterize(Vec2i(20, 34),   Vec2i(744, 400), render, red,   ybuffer);
        rasterize(Vec2i(120, 434), Vec2i(444, 400), render, green, ybuffer);
        rasterize(Vec2i(330, 463), Vec2i(594, 200), render, blue,  ybuffer);
        render.flip_vertically();
        render.write_tga_file("render.tga");
    }

    int *zbuffer = new int[width*height];


    return 0;
}
/*
 *     model = new Model("../obj/african_head.obj");
    for(int i = 0;i<model->nfaces();i++){
        std::vector<int> face = model->face(i);
        for(int j = 0;j<3;j++){
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;
            line(x0, y0, x1, y1, image, white);
        }
    }


    Vec2i t0[3] = {Vec2i(10, 10),   Vec2i(100, 30),  Vec2i(190, 160)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
    sort(t0,t0+3);
    sort(t1,t1+3);
    sort(t2,t2+3);

        model = new Model("../obj/african_head.obj");
    Vec3f light_dir = {0,0,-1};
    for(int i = 0;i<model->nfaces();i++){
        std::vector<int> face = model->face(i);
        Vec2i screen_cords[3];
        Vec3f world_cords[3];
        for(int j = 0;j<3;j++){
            Vec3f v0 = model->vert(face[j]);
            screen_cords[j] = Vec2i((v0.x+1.0)*width/2.,(v0.y+1.0)*height/2.);//屏幕坐标
            world_cords[j] = v0;
        }
        Vec3f n = (world_cords[2] - world_cords[0])^(world_cords[1] - world_cords[0]);//叉积
        n.normalize();
        float intensity = n*light_dir;
        //triangle(screen_cords[0], screen_cords[1], screen_cords[2], image, TGAColor(rand()%255, rand()%255, rand()%255, 255));
        if(intensity>0)triangle(screen_cords[0],screen_cords[1],screen_cords[2],image,TGAColor(intensity*255, intensity*255, intensity*255, 255));
    }
    image.flip_vertically();
    image.write_tga_file("color_LightModel.tga");

    delete model;
    }
 */