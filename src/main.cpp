#include <bits/stdc++.h>

#include "camera.hpp"
#include "image.hpp"
#include "object.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "texture.hpp"
#include "triangle.hpp"
#include "vecmath.h"

const static std::string path = "C:\\Users\\yfz\\Desktop\\data\\Plan\\homework\\ComputerGraphics\\proj\\code\\";

void addWall(Scene& S, Vector3f v1, Vector3f v2, Vector3f v3, Vector3f v4, std::shared_ptr<Material> m, std::shared_ptr<Color> c) {
    S.addObject(std::make_shared<Triangle>(v1, v2, v3, m, c));
    S.addObject(std::make_shared<Triangle>(v3, v4, v1, m, c));
}

void addLoliWall(Scene& S, Vector3f v1, Vector3f v2, Vector3f v3, Vector3f v4, std::shared_ptr<Material> m, std::shared_ptr<Image> img) {
    S.addObject(std::make_shared<Triangle>(v1, v2, v3, m, std::make_shared<TriangleTexture>(v1, v2, v3, Vector2f(0, 1), Vector2f(0, 0), Vector2f(1, 0), img)));
    S.addObject(std::make_shared<Triangle>(v3, v4, v1, m, std::make_shared<TriangleTexture>(v3, v4, v1, Vector2f(1, 0), Vector2f(1, 1), Vector2f(0, 1), img)));
}

void createBox(Scene& S, int X, int Y, int Z) {
    auto wallM = std::make_shared<Material>(Materials[MaterialType::DIFFUSE]);
    auto leftC = std::make_shared<Color>(0.25, 0.25, 0.75);
    auto rightC = std::make_shared<Color>(0.75, 0.25, 0.25);
    auto backC = std::make_shared<Color>(0.75, 0.75, 0.75);
    auto earthC = std::make_shared<Color>(0.75, 0.75, 0.25);
    auto blackC = std::make_shared<Color>(0.01, 0.91, 0.01);
    addWall(S, Vector3f(0, 0, 0), Vector3f(X, 0, 0), Vector3f(X, 0, Z), Vector3f(0, 0, Z), wallM, blackC);
    addWall(S, Vector3f(0, 0, 0), Vector3f(X, 0, 0), Vector3f(X, Y, 0), Vector3f(0, Y, 0), wallM, earthC);
    addWall(S, Vector3f(0, 0, 0), Vector3f(0, 0, Z), Vector3f(0, Y, Z), Vector3f(0, Y, 0), wallM, leftC);
    addWall(S, Vector3f(X, Y, Z), Vector3f(0, Y, Z), Vector3f(0, Y, 0), Vector3f(X, Y, 0), wallM, backC);
    addWall(S, Vector3f(X, Y, Z), Vector3f(0, Y, Z), Vector3f(0, 0, Z), Vector3f(X, 0, Z), wallM, backC);
    addWall(S, Vector3f(X, Y, Z), Vector3f(X, Y, 0), Vector3f(X, 0, 0), Vector3f(X, 0, Z), wallM, rightC);

    S.addLight(std::make_shared<CircleLight>(std::make_shared<Color>(1, 1, 1), Vector3f(2500, 2500, 2500) * Config::pi * 4, Vector3f(X * 0.5, Y * 0.5, Z - 1), 0.01, Vector3f(0, 0, -1)));
}

void createLoliBox(Scene& S, int X, int Y, int Z) {
    auto wallM = std::make_shared<Material>(Materials[MaterialType::DIFFUSE]);
    auto leftC = std::make_shared<Color>(0.25, 0.25, 0.75);
    auto rightC = std::make_shared<Color>(0.75, 0.25, 0.25);
    auto backC = std::make_shared<Color>(0.75, 0.75, 0.75);
    auto earthC = std::make_shared<Color>(0.75, 0.75, 0.25);
    auto blackC = std::make_shared<Color>(0.01, 0.01, 0.01);
    auto loliI = std::shared_ptr<Image>(Image::LoadTGA((path + "resource\\loli.tga").c_str()));
    addWall(S, Vector3f(0, 0, 0), Vector3f(X, 0, 0), Vector3f(X, 0, Z), Vector3f(0, 0, Z), wallM, blackC);
    addLoliWall(S, Vector3f(0, 0, 0), Vector3f(X, 0, 0), Vector3f(X, Y, 0), Vector3f(0, Y, 0), wallM, loliI);
    addWall(S, Vector3f(0, 0, 0), Vector3f(0, 0, Z), Vector3f(0, Y, Z), Vector3f(0, Y, 0), wallM, leftC);
    addLoliWall(S, Vector3f(X, Y, Z), Vector3f(0, Y, Z), Vector3f(0, Y, 0), Vector3f(X, Y, 0), wallM, loliI);
    addLoliWall(S, Vector3f(X, Y, Z), Vector3f(0, Y, Z), Vector3f(0, 0, Z), Vector3f(X, 0, Z), wallM, loliI);
    addWall(S, Vector3f(X, Y, Z), Vector3f(X, Y, 0), Vector3f(X, 0, 0), Vector3f(X, 0, Z), wallM, rightC);

    S.addLight(std::make_shared<CircleLight>(std::make_shared<Color>(1, 1, 1), Vector3f(2500, 2500, 2500) * Config::pi * 4, Vector3f(X * 0.5, Y * 0.5, Z - 1), 0.01, Vector3f(0, 0, -1)));
}

void createWhiteBox(Scene& S, int X, int Y, int Z) {
    auto wallM = std::make_shared<Material>(Materials[MaterialType::DIFFUSE]);
    auto whiteC = std::make_shared<Color>(0.99, 0.99, 0.99);
    addWall(S, Vector3f(0, 0, 0), Vector3f(X, 0, 0), Vector3f(X, 0, Z), Vector3f(0, 0, Z), wallM, whiteC);
    addWall(S, Vector3f(0, 0, 0), Vector3f(X, 0, 0), Vector3f(X, Y, 0), Vector3f(0, Y, 0), wallM, whiteC);
    addWall(S, Vector3f(0, 0, 0), Vector3f(0, 0, Z), Vector3f(0, Y, Z), Vector3f(0, Y, 0), wallM, whiteC);
    addWall(S, Vector3f(X, Y, Z), Vector3f(0, Y, Z), Vector3f(0, Y, 0), Vector3f(X, Y, 0), wallM, whiteC);
    addWall(S, Vector3f(X, Y, Z), Vector3f(0, Y, Z), Vector3f(0, 0, Z), Vector3f(X, 0, Z), wallM, whiteC);
    addWall(S, Vector3f(X, Y, Z), Vector3f(X, Y, 0), Vector3f(X, 0, 0), Vector3f(X, 0, Z), wallM, whiteC);

    S.addLight(std::make_shared<CircleLight>(std::make_shared<Color>(1, 1, 1), Vector3f(2500, 2500, 2500) * Config::pi * 4, Vector3f(X * 0.5, Y * 0.7, 50), 0.01, Vector3f(0, 0, -1)));
}

void createScene1(Scene& S) {
    S.addObject(std::make_shared<Sphere>(Vector3f(50, 50, 10), 10, std::make_shared<Material>(Materials[MaterialType::GLASS]), std::make_shared<Color>(0.999, 0.999, 0.999)));
    S.addObject(std::make_shared<Sphere>(Vector3f(15, 60, 8), 8, std::make_shared<Material>(Materials[MaterialType::MIRROR]), std::make_shared<Color>(0.999, 0.999, 0.999)));
}

void createScene2(Scene& S) {
    auto bunnyM = std::make_shared<Material>(Materials[MaterialType::GLASS]);
    auto bunnyC = std::make_shared<Color>(0.999, 0.999, 0.999);
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(path + "resource\\bunny_1k.obj", bunnyM, bunnyC);
    mesh->applyMat(Matrix4f::translation(Vector3f(0.03, -0.0666, 0)));
    mesh->applyMat(Matrix4f(Vector4f(1, 0, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 1, 0, 0), Vector4f(0, 0, 0, 0), false));
    mesh->transCenterTo(Vector3f(45, 45, 5), 40);
    mesh->transZTo(0, 1);
    // mesh->transCenterTo(Vector3f(20, 50, 20), 10);
    S.addMesh(mesh);
}

void createScene3(Scene& S) {
    auto bunnyM = std::make_shared<Material>(Materials[MaterialType::GLASS]);
    auto bunnyC = std::make_shared<Color>(0.999, 0.999, 0.999);
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(path + "resource\\cube.obj", bunnyM, bunnyC);
    mesh->applyMat(Matrix4f(Vector4f(1, 0, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 1, 0, 0), Vector4f(0, 0, 0, 0), false));
    mesh->transCenterTo(Vector3f(45, 40, 3), 5);
    mesh->transZTo(0, 1);
    // mesh->transCenterTo(Vector3f(20, 50, 20), 10);
    S.addMesh(mesh);
}

void createScene4(Scene& S) {
    auto bunnyM = std::make_shared<Material>(Materials[MaterialType::APPLE]);
    auto bunnyC = std::make_shared<Color>(0.999, 0.1, 0.1);
    // auto bunnyC = std::make_shared<Color>(0.999, 0.999, 0.999);
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(path + "resource\\food_apple_01_8k.obj", bunnyM, std::shared_ptr<Image>(Image::LoadPPM((path + "resource\\apple2.ppm").c_str())));
    mesh->applyMat(Matrix4f::rotateY(Config::pi / 2));
    mesh->applyMat(Matrix4f(Vector4f(1, 0, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 1, 0, 0), Vector4f(0, 0, 0, 0), false));
    mesh->transCenterTo(Vector3f(30, 50, 0), 3);
    mesh->transZTo(5, 1);
    // mesh->transCenterTo(Vector3f(20, 50, 20), 10);
    S.addMesh(mesh);
    // S.addObject(std::make_shared<Sphere>(Vector3f(55, 60, 8), 8, std::make_shared<Material>(Materials[MaterialType::MIRROR]), std::make_shared<Color>(0.999, 0.999, 0.999)));
}

void createScene5(Scene& S) {
    auto bunnyM = std::make_shared<Material>(Materials[MaterialType::MIRROR]);
    auto bunnyC = std::make_shared<Color>(0.6, 0.6, 0.6);
    std::shared_ptr<BsplineGroup> bsp = std::make_shared<BsplineGroup>(path + "resource\\vase.txt", Vector2f(0, 20), bunnyM, bunnyC);

    bsp->transformXY(Matrix3f::rotateZ(Config::pi / 2));
    bsp->transCenterTo(Vector3f(35, 45, 0), 0.8);
    // bsp->transCenterTo(Vector3f(10, 60, 10), 0.8);
    bsp->initCal();
    S.addBsplineGroup(bsp);
}


void createVase(Scene& S, double X, double Y, double ZL, double ZR, double alpha){
    auto bunnyM = std::make_shared<Material>(Materials[MaterialType::GLASS]);
    auto bunnyC = std::make_shared<Color>(0.999, 0.999, 0.999);
    std::shared_ptr<BsplineGroup> bsp = std::make_shared<BsplineGroup>(path + "resource\\vase.txt", Vector2f(ZL, ZR), bunnyM, bunnyC);
    std::shared_ptr<BsplineGroup> bsp2 = std::make_shared<BsplineGroup>(path + "resource\\vase.txt", Vector2f(ZL + (ZR - ZL) * 0.1, ZR), bunnyM, bunnyC);
    
    std::shared_ptr<BsplineGroup> top = std::make_shared<BsplineGroup>(Vector3f(X, Y, ZR), alpha, path + "resource\\vase.txt", Vector2f(ZR, ZR), bunnyM, bunnyC);
    std::shared_ptr<BsplineGroup> bottom = std::make_shared<BsplineGroup>(Vector3f(X, Y, ZL), Config::epsilon, path + "resource\\vase.txt", Vector2f(ZL, ZL), bunnyM, bunnyC);
    std::shared_ptr<BsplineGroup> bottom2 = std::make_shared<BsplineGroup>(Vector3f(X, Y, ZL + (ZR - ZL) * 0.1), Config::epsilon, path + "resource\\vase.txt", Vector2f(ZL + (ZR - ZL) * 0.1, ZL + (ZR - ZL) * 0.1), bunnyM, bunnyC);
    
    
    bsp->transformXY(Matrix3f::rotateZ(Config::pi / 2));
    bsp->transCenterTo(Vector3f(X, Y, ZL), 0.8);
    bsp->initCal();
    bsp2->transformXY(Matrix3f::rotateZ(Config::pi / 2));
    bsp2->transCenterTo(Vector3f(X, Y, ZL + (ZR - ZL) * 0.1), 0.8 * alpha);
    bsp2->initCal();
    top->transformXY(Matrix3f::rotateZ(Config::pi / 2));
    top->scaleXY(0.8);
    top->initCal();

    bottom->transformXY(Matrix3f::rotateZ(Config::pi / 2));
    bottom->scaleXY(0.8);
    bottom->initCal();

    bottom2->transformXY(Matrix3f::rotateZ(Config::pi / 2));
    bottom2->scaleXY(0.8 * alpha);
    bottom2->initCal();
    
    S.addBsplineGroup(bsp);
    // bsp->calcCenter().print();
    S.addBsplineGroup(bsp2);
    S.addBsplineGroup(top);
    S.addBsplineGroup(bottom);
    S.addBsplineGroup(bottom2);
    // bottom->calcCenter().print();
}

double createBook(Scene& S, double X, double Y, double Z) {
    auto bunnyM = std::make_shared<Material>(Materials[MaterialType::DIFFUSE]);
    // auto bunnyC = std::make_shared<Color>(0.999, 0.999, 0.999);
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(path + "resource\\book_final\\book.obj", bunnyM, path + "resource\\book_final\\book.ppm");
    // mesh->applyMat(Matrix4f(Vector4f(1, 0, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 1, 0, 0), Vector4f(0, 0, 0, 0), false));
    // mesh->applyMat(Matrix4f::rotateY(Config::pi));
    mesh->applyMat(Matrix4f(Vector4f(1, 0, 0, 0), Vector4f(0, 0.6, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 0, 0, 0), false));
    mesh->transCenterTo(Vector3f(X, Y, 0), 0.05);
    mesh->transZTo(Z, 1);
    double maxZ = mesh->calcHigh().z();
    // mesh->transCenterTo(Vector3f(20, 50, 20), 10);
    S.addMesh(mesh);
    return maxZ;
}

double createDesk(Scene& S, double X, double Y, double Z) {
    auto bunnyM = std::make_shared<Material>(Materials[MaterialType::DESK]);
    auto bunnyC = std::make_shared<Color>(0.999, 0.999, 0.999);
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(path + "resource\\desk\\desk.obj", bunnyM, path + "resource\\desk\\desk.ppm");
    mesh->applyMat(Matrix4f::rotateX(Config::pi / 2));
    mesh->applyMat(Matrix4f(Vector4f(10, 0, 0, 0), Vector4f(0, 2, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 0, 0, 0), false));
    mesh->transCenterTo(Vector3f(X, Y, 0), 50);
    mesh->transZTo(Z, 1);
    double maxZ = mesh->calcHigh().z();
    // mesh->transCenterTo(Vector3f(20, 50, 20), 10);
    S.addMesh(mesh);
    return maxZ;
}

void createApple(Scene& S, double X, double Y, double Z) {
    auto bunnyM = std::make_shared<Material>(Materials[MaterialType::APPLE]);
    auto bunnyC = std::make_shared<Color>(0.999, 0.1, 0.1);
    // auto bunnyC = std::make_shared<Color>(0.999, 0.999, 0.999);
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(path + "resource\\food_apple_01_8k.obj", bunnyM, std::shared_ptr<Image>(Image::LoadPPM((path + "resource\\apple2.ppm").c_str())));
    mesh->applyMat(Matrix4f::rotateY(Config::pi / 2));
    mesh->applyMat(Matrix4f(Vector4f(1, 0, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 1, 0, 0), Vector4f(0, 0, 0, 0), false));
    mesh->transCenterTo(Vector3f(X, Y, Z), 1);
    mesh->transZTo(Z, 1);
    // mesh->transCenterTo(Vector3f(20, 50, 20), 10);
    S.addMesh(mesh);
}

void createSphere(Scene& S, double X, double Y, double Z, double R){
    S.addObject(std::make_shared<Sphere>(Vector3f(X, Y, Z), R, std::make_shared<Material>(Materials[MaterialType::GLASS]), std::make_shared<Color>(0.999, 0.999, 0.999)));
}
void createSphereMirror(Scene& S, double X, double Y, double Z, double R){
    S.addObject(std::make_shared<Sphere>(Vector3f(X, Y, Z), R, std::make_shared<Material>(Materials[MaterialType::MIRROR]), std::make_shared<Color>(0.999, 0.999, 0.999)));
}

void createBackWall(Scene& S, double XL, double XR, double ZL, double ZR, double Y) {
    auto loliI = std::shared_ptr<Image>(Image::LoadPPM((path + "resource\\galaxy.ppm").c_str()));
    auto wallM = std::make_shared<Material>(Materials[MaterialType::DIFFUSE]);
    addLoliWall(S, Vector3f(XL, Y, ZR), Vector3f(XL, Y, ZL), Vector3f(XR, Y, ZL), Vector3f(XR, Y, ZR), wallM, loliI);
}

void createBackWhiteWall(Scene& S, double XL, double XR, double ZL, double ZR, double Y) {
    auto wallM = std::make_shared<Material>(Materials[MaterialType::DIFFUSE]);
    auto blackC = std::make_shared<Color>(0.99, 0.99, 0.99);
    addWall(S, Vector3f(XL, Y, ZR), Vector3f(XL, Y, ZL), Vector3f(XR, Y, ZL), Vector3f(XR, Y, ZR), wallM, blackC);
}

void createBunny(Scene& S, double X, double Y, double Z) {
    auto bunnyM = std::make_shared<Material>(Materials[MaterialType::GLASS]);
    auto bunnyC = std::make_shared<Color>(0.999, 0.699, 79/255.);
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(path + "resource\\stanford-bunny.obj", bunnyM, bunnyC);
    mesh->applyMat(Matrix4f(Vector4f(1, 0, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 1, 0, 0), Vector4f(0, 0, 0, 0), false));
    mesh->transCenterTo(Vector3f(X, Y, Z), 100);
    mesh->transZTo(Z, 1);
    // mesh->transCenterTo(Vector3f(20, 50, 20), 10);
    S.addMesh(mesh);
}


void createBunny2(Scene& S, double X, double Y, double Z) {
    auto bunnyM = std::make_shared<Material>(Materials[MaterialType::GLASS]);
    auto bunnyC = std::make_shared<Color>(0.999, 0.699, 79/255.);
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(path + "resource\\stanford-bunny.obj", bunnyM, bunnyC);
    mesh->applyMat(Matrix4f(Vector4f(1, 0, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 1, 0, 0), Vector4f(0, 0, 0, 0), false));
    mesh->transCenterTo(Vector3f(X, Y, Z), 10);
    mesh->transZTo(Z, 1);
    // mesh->transCenterTo(Vector3f(20, 50, 20), 10);
    S.addMesh(mesh);
}

void createFinalScene1(Scene& S) {
    S.addLight(std::make_shared<CircleLight>(std::make_shared<Color>(1, 1, 1), Vector3f(6500, 6500, 6500) * Config::pi * 4, Vector3f(120, 190, 60), 0.01, Vector3f(0, 0, -1)));

    double deskZ = createDesk(S, 100, 190, 0);

    createVase(S, 80, 195, deskZ, deskZ + 10, 0.9);

    createBook(S, 120, 190, deskZ);

    createApple(S, 100, 190, deskZ);

    createSphere(S, 80, 180, deskZ + 5, 5);
    createSphereMirror(S, 140, 210, deskZ + 10, 10);

    // 140, 140/3*2=100
    createBackWall(S, 30, 170, deskZ - 30, deskZ + 70, 220);

    createBunny(S, 110, 205, deskZ);

}

double createFinalScene2(Scene& S) {
    S.addLight(std::make_shared<CircleLight>(std::make_shared<Color>(1, 1, 1), Vector3f(6500, 6500, 6500) * Config::pi * 4, Vector3f(120, 190, 60), 0.01, Vector3f(0, 0, -1)));

    double deskZ = createDesk(S, 100, 190, 0);

    createVase(S, 80, 195, deskZ, deskZ + 10, 0.9);

    double bookZ = createBook(S, 120, 190, deskZ);

    createApple(S, 100, 190, deskZ);

    createSphere(S, 80, 180, deskZ + 5, 5);
    createSphereMirror(S, 140, 205, deskZ + 10, 10);

    // 140, 140/3*2=100
    createBackWall(S, 30, 170, deskZ - 30, deskZ + 70, 220);

    createBunny(S, 100, 205, deskZ);

    return deskZ;

    // createBackWhiteWall(S, -1000000, 1000000, -1000000, 1000000, 120);

}

void createMiku(Scene& S, double X, double Y, double Z) {
    std::shared_ptr<MaterialReader> reader = std::make_shared<MaterialReader>(1., 0., 0., 1.5, path + "resource\\miku\\", path + "resource\\miku\\append.mtl");
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(path + "resource\\miku\\append.obj", reader);
    mesh->applyMat(Matrix4f::rotateX(Config::pi / 2));
    mesh->applyMat(Matrix4f(Vector4f(1, 0, 0, 0), Vector4f(0, 1, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 0, 0, 0), false));
    mesh->transCenterTo(Vector3f(X, Y, Z), 10);
    mesh->transZTo(Z, 1);
    // mesh->transCenterTo(Vector3f(20, 50, 20), 10);
    S.addMesh(mesh);
}

void createFinalScene3(Scene& S, double X, double Y, double Z) {
    std::shared_ptr<MaterialReader> reader = std::make_shared<MaterialReader>(0., 1., 0., 1.5, "", path + "resource\\final2\\final5.mtl");
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(path + "resource\\final2\\final5.obj", reader);
    mesh->calcCenter().print();
    // mesh->applyMat(Matrix4f::rotateX(Config::pi / 2));
    // mesh->applyMat(500 * Matrix4f(Vector4f(1, 0, 0, 0), Vector4f(0, 1, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 0, 0, 0), false));
    mesh->transCenterTo(Vector3f(X, Y, Z), 0.05);
    // mesh->transZTo(Z + 20, 1);
    // mesh->transCenterTo(Vector3f(20, 50, 20), 10);
    S.addMesh(mesh);
}


int main() {
    int imgW = 1000, imgH = 1000;

    // imgW *= 0.25, imgH *= 0.25;
    // auto camera = new PerspectiveCamera(Vector3f(35, 35, 40), Vector3f(0, 1, -1.).normalized(), Vector3f(0, 1., 1.).normalized(), imgW, imgH, 120 * Config::pi / 180.);
    // auto camera = new PerspectiveCamera(Vector3f(35, 5, 20), Vector3f(0, 1, 0.).normalized(), Vector3f(0, 0, 1.).normalized(), imgW, imgH, 90 * Config::pi / 180.);
    
    
    
    Scene S;

    // double deskZ = createFinalScene1(S);
    createFinalScene1(S);


    auto camera = new PerspectiveCamera(Vector3f(100, 140, 70), Vector3f(0, 2, -1.).normalized(), Vector3f(0, 2, 1).normalized(), imgW, imgH, 80 * Config::pi / 180.);
    
    
    // auto camera = new DepthFieldCamera(Vector3f(100, 140, 70), Vector3f(100, 190, deskZ + 10), Vector3f(0, 0, 1).normalized(), imgW, imgH, 80 * Config::pi / 180., 4);

    
    S.setPicSize(camera->getWidth(), camera->getHeight());
    S.initAll(200, 200, 100);

    Image output_img(camera->getWidth(), camera->getHeight());
    /*
    // Vector3f lookfrom = Vector3f(-20, 10, -10);
    Vector3f lookfrom = Vector3f(-11, 5, -30);
    Vector3f lookat = Vector3f(-5, 0, 0);

    lookfrom -= Vector3f(lookat - lookfrom).normalized() * 5;

    auto camera = new PerspectiveCamera(lookfrom, Vector3f(lookat - lookfrom).normalized(), Vector3f(0, 0, 1).normalized(), imgW, imgH, 120 * Config::pi / 180.);
    
    Scene S;
    S.setPicSize(camera->getWidth(), camera->getHeight());
    S.initAll(200, 200, 70);
    */
    //createWhiteBox(S, 200, 250, 100);
    // createLoliBox(S, 70, 70, 40);
    
    // createScene1(S);
    // createScene4(S);
    // createScene2(S);
    // createScene4(S);
    // S.addObject(std::make_shared<Sphere>(Vector3f(40, 50, 10), 10, std::make_shared<Material>(Materials[MaterialType::GLASS]), std::make_shared<Color>(0.999, 0.999, 0.999)));

    // createScene5(S);

    //createVase(S, 30, 50, 1, 7, 0.95);

    // createBook(S, 50, 50, 1);

    // createFinalScene2(S);

    
    /*
    S.addLight(std::make_shared<CircleLight>(std::make_shared<Color>(1, 1, 1), Vector3f(4500, 4500, 4500) * Config::pi * 4, Vector3f(0, 0, -10), 0.01, Vector3f(0, 0, -1)));

    // double deskZ = createDesk(S, 100, 190, 0);

    // createBackWall(S, 30, 170, deskZ - 30, deskZ + 70, 220);

    // createMiku(S, 100, 190, deskZ);

    createFinalScene3(S, 0, 0, 0);

    */
    
    S.initObjectKDTree();
    // int numR = 1, numPhoton = 1;
    int numR = 1000;
    int numPhoton = 1e6;
    int cnt = 10, cnt0 = 10;
    for (int T = 0; T < numR; T++) {
        fprintf(stderr, "Round %d/%d\n", T, numR);
        S.initRound();
        
 #pragma omp parallel for schedule(dynamic, 1)
        for (int i = 0; i < camera->getWidth(); i++) {
            for (int j = 0; j < camera->getHeight(); j++) {
                Ray cam_ray = camera->generateRay(Vector2f(i + MyUtils::random01() - 0.5, j + MyUtils::random01() - 0.5));
                S.trace(cam_ray, Vector3f::ZERO, Vector3f(1, 1, 1), 1, cnt + i * camera->getHeight() + j, TraceType::EYE, S.getHit(i, j));
            }
        }
        /*
        for (int i = 132; i <= 132; i++) {
            for (int j = 228; j <= 228; j++) {
                Ray cam_ray = camera->generateRay(Vector2f(i, j));
                S.trace(cam_ray, Vector3f::ZERO, Vector3f(1, 1, 1), 1, ++cnt, TraceType::EYE, S.getHit(i, j));
            }
        }
        
        int i = 275, j = 245;
        Ray cam_ray = camera->generateRay(Vector2f(i, j));
        S.trace(cam_ray, Vector3f::ZERO, Vector3f(1, 1, 1), 1, ++cnt, TraceType::EYE, S.getHit(i, j));
        */
        fprintf(stderr, "Ray tracing pass complete.\n");
        cnt += camera->getWidth() * camera->getHeight();
        //S.buildHitKDTree();
        S.buildHashGrid();
#pragma omp parallel for schedule(dynamic, 1)
        for (int i = 0; i < numPhoton; i++) {
            Ray ray = S.genRandomRay(cnt + i);
            // if (i % 1000 == 0) fprintf(stderr, "<ray %d>\n", i);
            S.trace(ray, S.getLightWeight(), S.getLightColor(), 1, cnt0 + i, TraceType::PHOTON, nullptr);
        }
        fprintf(stderr, "[%d]", S.CNT);
        fprintf(stderr, "Photon pass complete.\n");
        cnt0 += numPhoton;
        cnt += numPhoton;
        S.updateAllFlux();
        
        if(T <= 10 || T % 50 == 0) {
            for (int i = 0; i < camera->getWidth(); ++i)
                for (int j = 0; j < camera->getHeight(); ++j) {
                    int id = i * camera->getHeight() + j;
                    Vector3f pixel = S.getFlux(id, T + 1, numPhoton);
                    for (int k = 0; k < 3; ++k) pixel[k] = std::min(1., pow(pixel[k], Config::gamma));
                    output_img.SetPixel(i, j, pixel);
                }

            output_img.SaveImage((path + "a" + std::to_string(T) + ".bmp").c_str());
        }
    }

    output_img.SaveImage((path + "a.bmp").c_str());
    return 0;
}