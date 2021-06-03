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
#define fuck(x) cout<<x<<endl
Vec3f cross(Vec3f A,Vec3f B){
    return Vec3f(A.y*B.z - B.y*A.z,-(A.x*B.z-B.x*A.z),A.x*B.y-B.x*A.y);
}

Vec3f Compute(Vec3f* pts,Vec3f p){
    Vec3f s[2];
    for(int i = 2;i--;){
        s[i][0] = pts[2][i] - pts[0][i];
        s[i][1] = pts[1][i] - pts[0][i];
        s[i][2] = pts[0][i] - p[i];
    }
    Vec3f u = cross(s[0],s[1]);
    if(std::abs(u[2])>1e-2)
        return Vec3f(1.f - (u.x+u.y)/u.z,u.y/u.z,u.x/u.z);
    return Vec3f(-1,1,1);
}

void barycentric(Vec3f* pts,Vec3f* tex,float* zbuffer,TGAImage& image,TGAImage& image1){//插值，包围框内选点
    Vec2f MinBox(image.get_width()-1,image.get_height()-1);
    Vec2f MaxBox(0,0);
    for(int i = 0;i<3;i++){
        MinBox.x = max(0.0f,min(MinBox.x,pts[i].x));
        MinBox.y = max(0.0f,min(MinBox.y,pts[i].y));
        MaxBox.x = min(image.get_width()-1.0f,max(MaxBox.x,pts[i].x));
        MaxBox.y = min(image.get_height()-1.0f,max(MaxBox.y,pts[i].y));
    }
    for(int i = MinBox.x;i<=MaxBox.x;i++){
        for(int j = MinBox.y;j<=MaxBox.y;j++){
            Vec3f temp = Compute(pts,Vec3f(i,j,0));//返回的1-(u+v),u,v 用这三个值对三角形的三个顶点进行插值，纹理同理
            if(temp.x<0||temp.y<0||temp.z<0)continue;
            float z = 0;
            float px = 0;
            float py = 0;
            for(int k = 0;k<3;k++)z += pts[k][2]*temp[k], px+=tex[k][0]*temp[k],py+=tex[k][1]*temp[k];
            px*=(image1.get_width()-1);
            py*=(image1.get_height()-1);
          //  cout<<px<<" "<<py<<endl;
            if(zbuffer[int(i+j*width)]<z){
                zbuffer[int(i+j*width)] = z;
                image.set(i,j,image1.get(px,py));
            }
        }
    }
}

void triangle(Vec2i t0,Vec2i t1,Vec2i t2,TGAImage& image, TGAColor color){
    //sweepLine(t0,t1,t2,image,color);
    Vec2i pts[3] = {t0,t1,t2};
    //barycentric(pts,image,color);
}
Vec3f world2screen(Vec3f v){
    return Vec3f (int ((v.x+1.0)*width/2.+5),int((v.y+1.)*height/2.+5),v.z);
}

int main() {
    TGAImage image1;
    image1.read_tga_file("../obj/african_head_diffuse.tga");
    image1.flip_vertically();
    float *zbuffer = new float [width*height];
    for(int i = width*height-1;i>=0;i--){
        zbuffer[i] = -std::numeric_limits<float>::max();
    }
    TGAImage image(width,height,TGAImage::RGB);
    model = new Model("../obj/african_head.obj");
    Vec3f light_dir = {0,0,-1};
    for(int i = 0;i<model->nfaces();i++){
        std::vector<int> face = model->face(i);
        std::vector<int> texture_face = model->texture(i);
        Vec3f pts[3];
        Vec3f tex[3];
        for(int j = 0;j<3;j++){
            Vec3f v0 = model->vert(face[j]);
            pts[j] = world2screen(v0);
            tex[j] = model->texture_vert(texture_face[j]);
        }
        barycentric(pts,tex,zbuffer, image, image1);
    }
    image.flip_vertically();
    image.write_tga_file("texture.tga");

    delete model;
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


 */