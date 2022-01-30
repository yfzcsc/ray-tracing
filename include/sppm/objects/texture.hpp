#ifndef _TEXTURE_H__
#define _TEXTURE_H__
#include <memory>
#include <string>

#include "color.hpp"
#include "config.hpp"
#include "image.hpp"

class Texture : public Color {
   public:
    Texture() = delete;
    Texture(std::shared_ptr<Image> _I) { load(_I); }
    virtual ColorType type() const override { return ColorType::TEXTURE; };

    void load(std::shared_ptr<Image> _I) { I = _I; };

   protected:
    std::shared_ptr<Image> I;
};

class TriangleTexture : public Texture {
   public:
    TriangleTexture() = delete;

    TriangleTexture(Vector3f xa, Vector3f xb, Vector3f xc, Vector2f a, Vector2f b, Vector2f c, std::shared_ptr<Image> _I) : Texture(_I) {
        vetx[0] = a;
        vetx[1] = b;
        vetx[2] = c;
        trivx[0] = xa;
        trivx[1] = xb;
        trivx[2] = xc;
        calIvMat();
    }

    Vector3f getColor(const Vector3f& v) const override {
        Vector3f A = (v - trivx[0]);
        // printf("[%d]---", flag);
        // v.print();
        // for (int i = 0; i < 3; ++i) trivx[i].print();
        // fflush(stdout);
        Vector2f xy = ivmat * (flag == 1 ? A.xy() : (flag == 2 ? A.yz() : A.xz()));
        double x = xy.x(), y = xy.y();
        Vector2f pos = vetx[1] * x + vetx[2] * y + vetx[0] * (1 - x - y);
        // Vector3f _A = trivx[1] * x + trivx[2] * y + trivx[0] * (1 - x - y);
        // printf("[%.6lf, %.6lf, %.6lf, %.6lf, %.6lf]\n", x, y, pos.x(), pos.y(), (v - _A).squaredLength());
        // fflush(stdout);
        return I->GetPixel(std::min(1., pos.x()), std::min(1., pos.y()));
    }

    void applyMat(const Matrix4f& mat) override {
        for (int i = 0; i < 3; ++i) trivx[i] = (mat * Vector4f(trivx[i], 1)).xyz();
        calIvMat();
    }

    void calIvMat() {
        Vector3f _v1 = trivx[1] - trivx[0];
        Vector3f _v2 = trivx[2] - trivx[0];
        flag = 0;
        auto v1 = _v1.xy();
        auto v2 = _v2.xy();
        if (fabs(v1.x() * v2.y() - v2.x() * v1.y()) > Config::epsilon) {
            flag = 1;
            ivmat = Matrix2f(v2.y(), -v2.x(), -v1.y(), v1.x()) * (1. / (v1.x() * v2.y() - v2.x() * v1.y()));
            return;
        }
        v1 = _v1.yz();
        v2 = _v2.yz();
        if (fabs(v1.x() * v2.y() - v2.x() * v1.y()) > Config::epsilon) {
            flag = 2;
            ivmat = Matrix2f(v2.y(), -v2.x(), -v1.y(), v1.x()) * (1. / (v1.x() * v2.y() - v2.x() * v1.y()));
            return;
        }
        v1 = _v1.xz();
        v2 = _v2.xz();
        if (fabs(v1.x() * v2.y() - v2.x() * v1.y()) > Config::epsilon) {
            flag = 3;
            ivmat = Matrix2f(v2.y(), -v2.x(), -v1.y(), v1.x()) * (1. / (v1.x() * v2.y() - v2.x() * v1.y()));
            return;
        }
    }

   private:
    Vector2f vetx[3];
    Vector3f trivx[3];
    Matrix2f ivmat;
    int flag;
};

#endif