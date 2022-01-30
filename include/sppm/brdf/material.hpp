#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <cmath>

#include "vecmath/vecmath.h"

class Material {
   public:
    double specular, diffuse, refraction;
    Vector3f Ka, Kd, Ks;
    double phong_s, refractiveIndex;
    // specular: 反射系数
    // diffuse: 漫反射系数
    // refraction: 折射系数
    // refractionIndex: 折射率
    // phong模型: rho_d, rho_s, phong_s
    Material() {}
    Material(double specular, double diffuse, double refraction, double Kd, double Ks, double phong_s, double refractiveIndex) {
        this->specular = specular;
        this->diffuse = diffuse;
        this->refraction = refraction;
        this->Ka = Vector3f::ZERO;
        this->Kd = Vector3f(1., 1., 1.) * Kd;
        this->Ks = Vector3f(1., 1., 1.) * Ks;
        this->phong_s = phong_s;
        this->refractiveIndex = refractiveIndex;
    }
    
    Material(double specular, double diffuse, double refraction, Vector3f Ka, Vector3f Kd, Vector3f Ks, double phong_s, double refractiveIndex) {
        this->specular = specular;
        this->diffuse = diffuse;
        this->refraction = refraction;
        this->Ka = Ka;
        this->Kd = Kd;
        this->Ks = Ks;
        this->phong_s = phong_s;
        this->refractiveIndex = refractiveIndex;
    }
    // phong f(w_i \to w_r)
    Vector3f cal(const Vector3f& dirFromLight, const Vector3f& hitNormal, const Vector3f& dirFromEye) {
        auto dir_normalized = -dirFromEye.normalized();
        auto R = (dirFromLight - 2 * Vector3f::dot(hitNormal, dirFromLight) * hitNormal).normalized();

        auto clamp = [](double x) { return x < 0 ? 0 : x; };
        Vector3f rho = Kd + Ks * pow(clamp(Vector3f::dot(R, dir_normalized)), phong_s);

        return rho;
    }
};

enum MaterialType { DIFFUSE = 0, MIRROR, GLASS, MLIGHT, BUNNY, APPLE, DESK };

const Material Materials[] = {
    Material(0, 1, 0, 0.7, 0, 0, 0),            // DIFFUSE
    Material(1, 0, 0, 0, 0, 0, 0),              // MIRROR
    Material(0, 0.0, 1, 0, 0, 0, 1.5),           // GLASS
    Material(0, 1, 0, 0, 0, 0, 0),              // MLIGHT
    Material(0.2, 0.3, 0.5, 0.4, 0.7, 0, 1.5),  // BUNNY
    // Material(0.05, 0.75, 0.2, 0.4, 0.7, 0, 1.5),  // APPLE
    Material(0.05, 0.85, 0.0, 0.7, 0.4, 10, 0.),  // APPLE
    Material(0.05, 0.95, 0.0, 0.7, 0., 0, 0.),  // DESK
};

#endif  // MATERIAL_H