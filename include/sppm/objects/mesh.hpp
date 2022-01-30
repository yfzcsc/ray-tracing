#ifndef MESH_H
#define MESH_H

#include <algorithm>
#include <string>
#include <vector>

#include "image.hpp"
#include "object.hpp"
#include "texture.hpp"
#include "triangle.hpp"
#include "material_read.hpp"

class MeshTriangle : public Triangle {
   public:
    MeshTriangle() = delete;
    MeshTriangle(const Vector3f& a, const Vector3f& b, const Vector3f& c, std::shared_ptr<Material> m, std::shared_ptr<Color> col, bool flag, const Vector3f& na, const Vector3f& nb, const Vector3f& nc) : Triangle(a, b, c, m, col) {
        new_normal[0] = na;
        new_normal[1] = nb;
        new_normal[2] = nc;
        use_new_normal = flag;
    }
};

class Mesh {
   public:
    Mesh() {}
    Mesh(std::string filename, std::shared_ptr<Material> material, std::shared_ptr<Color> col) : material(material), col(col) { load(filename); }
    Mesh(std::string filename, std::shared_ptr<Material> material, std::shared_ptr<Image> _I) : material(material), imgI(_I) { load(filename); }
    Mesh(std::string filename, std::shared_ptr<Material> material, std::string imgfile) : material(material), imgI(Image::LoadPPM(imgfile.c_str())) { load(filename); }
    Mesh(std::string filename, std::shared_ptr<MaterialReader> mtl) { load(filename, mtl); }
    void load(std::string filename, std::shared_ptr<MaterialReader> mtl = std::shared_ptr<MaterialReader>(nullptr));

    Vector3f calcCenter();
    Vector3f calcLow();
    Vector3f calcHigh();
    void transCenterTo(const Vector3f& newCenter, double scale);
    void transZTo(const double& newZ, double scale);
    void applyMat(const Matrix4f& mat);

    std::vector<MeshTriangle> triangles;
    std::vector<Vector3f> vetx;
    std::vector<Vector3f> vetxn;
    std::vector<Vector2f> uv;
    std::shared_ptr<Material> material;
    std::shared_ptr<Color> col;
    std::shared_ptr<Image> imgI;
};

#endif
