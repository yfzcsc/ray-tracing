#ifndef HIT_H
#define HIT_H

#include <vecmath.h>

#include <memory>

#include "material.hpp"
#include "ray.hpp"

class Hit {
   public:
    // constructors
    Hit() {
        material = nullptr;
        t = 1e38;
    }

    Hit(double _t, std::shared_ptr<Material> m, const Vector3f &n) {
        t = _t;
        material = m;
        normal = n;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
    }

    // destructor
    ~Hit() = default;

    double getT() const { return t; }

    std::shared_ptr<Material> getMaterial() const { return material; }

    const Vector3f &getNormal() const { return normal; }

    void set(double _t, std::shared_ptr<Material> m, const Vector3f &n) {
        t = _t;
        material = m;
        normal = n;
    }

    double diffuse() { return material->diffuse; }
    double specular() { return material->specular; }

    double refraction() { return material->refraction; }
    double refractiveIndex() { return material->refractiveIndex; }
    double total_light() { return material->diffuse + material->specular + material->refraction; }

    Vector3f Kd() { return material->Kd; }
    Vector3f Ks() { return material->Ks; }
    Vector3f Ka() { return material->Ka; }

    double phong_s() { return material->phong_s; }

    Vector3f getNormal() { return normal; }

    bool valid() { return material.get() != nullptr; }

   private:
    double t;
    std::shared_ptr<Material> material;
    Vector3f normal;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif  // HIT_H
