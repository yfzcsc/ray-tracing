#include "mesh.hpp"

#include <fstream>
#include <sstream>
#include <vector>

void Mesh::load(std::string filename, std::shared_ptr<MaterialReader> mtl) {
    triangles.clear();
    std::ifstream in(filename);
    std::string line;
    if (!in.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    vetx.clear();
    vetxn.clear();
    uv.clear();
    std::string nwmtl = "";
    while (!in.eof()) {
        std::getline(in, line);
        // puts(line.c_str());
        stringstream ss(line);
        std::string token;
        ss >> token;
        if (token == "v") {
            double x0, x1, x2;
            ss >> x0 >> x1 >> x2;
            vetx.push_back(Vector3f(x0, x1, x2));
        } else if (token == "vn") {
            double x0, x1, x2;
            ss >> x0 >> x1 >> x2;
            vetxn.push_back(Vector3f(x0, x1, x2));
        } else if (token == "vt") {
            double x0, x1, x2;
            ss >> x0 >> x1 >> x2;
            uv.push_back(Vector2f(x0, x1));
        } else if (token == "usemtl") {
            ss >> nwmtl;
        } else if (token == "f") {
            std::vector<std::string> vs;
            std::string v0;
            while(ss >> v0) vs.push_back(v0);
            auto get = [](std::string x) {
                std::vector<int> A;
                for (auto& a : x)
                    if (a == '/') a = ' ';
                stringstream ss2(x);
                int z;
                while (ss2 >> z) A.push_back(z);
                return A;
            };
            int fcnt = 0;
            for (auto& a : vs[0])
                if (a == '/') fcnt++;
            v0 = vs[0];
            for(int i = 1; i < vs.size() - 1; i = i + 1) {
                std::vector<int> va, vb, vc;
                va = get(v0), vb = get(vs[i]), vc = get(vs[i + 1]);
                if (fcnt == 0) {
                    triangles.push_back(MeshTriangle(vetx[va[0] - 1], vetx[vb[0] - 1], vetx[vc[0] - 1], mtl == nullptr || mtl->getMaterial(nwmtl) == nullptr ? material : mtl->getMaterial(nwmtl), col, false, Vector3f::ZERO, Vector3f::ZERO, Vector3f::ZERO));
                } else if (fcnt == 1) {
                    auto _col = std::make_shared<TriangleTexture>(vetx[va[0] - 1], vetx[vb[0] - 1], vetx[vc[0] - 1], uv[va[1] - 1], uv[vb[1] - 1], uv[vc[1] - 1], nwmtl == "" || mtl == nullptr || mtl->getColFile(nwmtl) == nullptr ? imgI : mtl->getColFile(nwmtl));
                    triangles.push_back(MeshTriangle(vetx[va[0] - 1], vetx[vb[0] - 1], vetx[vc[0] - 1], mtl == nullptr || mtl->getMaterial(nwmtl) == nullptr ? material : mtl->getMaterial(nwmtl), _col, false, Vector3f::ZERO, Vector3f::ZERO, Vector3f::ZERO));
                } else if (fcnt == 2) {
                    auto _col = imgI == nullptr && (nwmtl == "" || mtl == nullptr || mtl->getColFile(nwmtl) == nullptr) ? std::make_shared<Color>(0.99, 0.99, 0.99) : std::make_shared<TriangleTexture>(vetx[va[0] - 1], vetx[vb[0] - 1], vetx[vc[0] - 1], uv[va[1] - 1], uv[vb[1] - 1], uv[vc[1] - 1], nwmtl == "" || mtl == nullptr || mtl->getColFile(nwmtl) == nullptr ? imgI : mtl->getColFile(nwmtl));
                    triangles.push_back(MeshTriangle(vetx[va[0] - 1], vetx[vb[0] - 1], vetx[vc[0] - 1], mtl == nullptr || mtl->getMaterial(nwmtl) == nullptr ? material : mtl->getMaterial(nwmtl), _col, true, vetxn[va.back() - 1], vetxn[vb.back() - 1], vetxn[vc.back() - 1]));
                }
            }
            
        }
    }
    in.close();
}

Vector3f Mesh::calcCenter() {
    Vector3f ret = Vector3f::ZERO;
    for (auto a : vetx) ret += a;
    return ret / vetx.size();
}
Vector3f Mesh::calcLow() {
    if (!vetx.size()) return Vector3f::ZERO;
    Vector3f ret = vetx[0];
    for (auto a : vetx) {
        for (int i = 0; i < 3; ++i) ret[i] = std::min(ret[i], a[i]);
    }
    return ret;
}
Vector3f Mesh::calcHigh() {
    if (!vetx.size()) return Vector3f::ZERO;
    Vector3f ret = vetx[0];
    for (auto a : vetx) {
        for (int i = 0; i < 3; ++i) ret[i] = std::max(ret[i], a[i]);
    }
    return ret;
}

void Mesh::transCenterTo(const Vector3f& newCenter, double scale) {
    calcCenter().print();
    Vector3f cen = calcCenter();
    Vector3f vp = -cen * scale + newCenter;
    Matrix4f M(Vector4f(scale, 0, 0, vp.x()), Vector4f(0, scale, 0, vp.y()), Vector4f(0, 0, scale, vp.z()), Vector4f(0, 0, 0, 0), false);
    applyMat(M);
}

void Mesh::transZTo(const double& newZ, double scale) {
    calcLow().print();
    Vector3f vp = Vector3f(0., 0., newZ - calcLow().z() * scale);
    Matrix4f M(Vector4f(scale, 0, 0, vp.x()), Vector4f(0, scale, 0, vp.y()), Vector4f(0, 0, scale, vp.z()), Vector4f(0, 0, 0, 0), false);
    applyMat(M);
}
void Mesh::applyMat(const Matrix4f& M) {
    for (auto& a : triangles) a.transform(M);
    for (auto& a : vetx) a = (M * Vector4f(a, 1)).xyz();
    for (auto& a : vetxn) a = ((M * Vector4f(a, 0)).xyz()).normalized();
}