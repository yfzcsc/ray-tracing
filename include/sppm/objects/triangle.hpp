#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vecmath.h>

#include <algorithm>
#include <cmath>
#include <iostream>

#include "hit.hpp"
#include "object.hpp"
#include "ray.hpp"
using namespace std;

class Triangle : public Object {
   public:
    Triangle() = delete;

    // a b c are three vertex positions of the triangle
    Triangle(const Vector3f& a, const Vector3f& b, const Vector3f& c, std::shared_ptr<Material> m, std::shared_ptr<Color> col) : Object(m, col) {
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
        use_new_normal = false;
        Vector3f e1 = vertices[1] - vertices[0], e2 = vertices[2] - vertices[0];
        normal = Vector3f(e1[1] * e2[2] - e1[2] * e2[1], e1[2] * e2[0] - e1[0] * e2[2], e1[0] * e2[1] - e1[1] * e2[0]).normalized();
        _type = ObjectType::THING;
    }
    bool intersect(const Ray& ray, Hit& hit, double tMin) override {
        auto det33 = [](Vector3f a0, Vector3f a1, Vector3f a2) {
            double ans = 0;
#define det22(x0, x1, y0, y1) ((a##x0[y0]) * (a##x1[y1]) - (a##x0[y1]) * (a##x1[y0]))
            ans += a0[0] * det22(1, 2, 1, 2);
            ans -= a0[1] * det22(1, 2, 0, 2);
            ans += a0[2] * det22(1, 2, 0, 1);
#undef det22
            return ans;
        };
        Vector3f rd = ray.getDirection(), e1 = vertices[0] - vertices[1], e2 = vertices[0] - vertices[2], s = vertices[0] - ray.getOrigin();

        double X = det33(rd, e1, e2);
        if (fabs(X) <= 1e-9) return false;
        double t = det33(s, e1, e2) / X, beta = det33(rd, s, e2) / X, gamma = det33(rd, e1, s) / X;
        // puts("TRIANGLE OK1");
        // for (int i = 0; i < 3; ++i) vertices[i].print();
        // printf("[%.6lf, %.6lf, %.6lf]", beta, gamma, t);
        if (beta + gamma > 1 + Config::epsilon || t < tMin || beta < -Config::epsilon || gamma < -Config::epsilon || t >= hit.getT()) return false;
        // puts("TRIANGLE OK2");

        Vector3f vec_n = getNormal(1 - beta - gamma, beta, gamma);
        if (Vector3f::dot(vec_n, rd) > 0) vec_n.negate();
        // print();
        // printf("vec_n:");
        // vec_n.print();
        hit.set(t, material, vec_n);
        return true;
    }
    ObjectBox getObjectBox() override {
        ObjectBox ret;
        ret.x_range = Vector2f(std::min({vertices[0].x(), vertices[1].x(), vertices[2].x()}), std::max({vertices[0].x(), vertices[1].x(), vertices[2].x()}));
        ret.y_range = Vector2f(std::min({vertices[0].y(), vertices[1].y(), vertices[2].y()}), std::max({vertices[0].y(), vertices[1].y(), vertices[2].y()}));
        ret.z_range = Vector2f(std::min({vertices[0].z(), vertices[1].z(), vertices[2].z()}), std::max({vertices[0].z(), vertices[1].z(), vertices[2].z()}));
        return ret;
    }

    Vector3f getNormal(double alpha, double beta, double gamma) {
        if (use_new_normal) {
            return alpha * new_normal[0] + beta * new_normal[1] + gamma * new_normal[2];
        }
        return normal;
    }

    void transform(const Matrix4f& mat) {
        for (int i = 0; i < 3; ++i) {
            vertices[i] = (mat * Vector4f(vertices[i], 1)).xyz();
        }
        normal = ((mat * Vector4f(normal, 0)).xyz()).normalized();
        if (use_new_normal) {
            for (int i = 0; i < 3; ++i) {
                new_normal[i] = ((mat * Vector4f(new_normal[i], 0)).xyz()).normalized();
            }
        }
        color->applyMat(mat);
    }

    void print() {
        for (int i = 0; i < 3; ++i) vertices[i].print();
    }

   protected:
    Vector3f normal;
    Vector3f vertices[3];
    Vector3f new_normal[3];
    bool use_new_normal;
};

#endif  // TRIANGLE_H
