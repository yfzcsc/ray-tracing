#ifndef SPHERE_H
#define SPHERE_H

#include <cmath>

#include "hit.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "vecmath.h"

class Sphere : public Object {
   public:
    Sphere() = delete;

    Sphere(const Vector3f &center, double radius, std::shared_ptr<Material> material, std::shared_ptr<Color> c) : Object(material, c), center(center), radius(radius) { _type = ObjectType::THING; }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, double tMin) override {
        // Suppose origin is O, center is C.
        Vector3f OtoC = center - r.getOrigin();
        double t, b = Vector3f::dot(OtoC, r.getDirection());
        // puts("SPERE OK");
        // r.getDirection().print();
        // r.getOrigin().print();
        double det = b * b - OtoC.squaredLength() + radius * radius;
        if (det < 0) return false;
        // puts("SPERE OK2");
        det = sqrt(det);
        if (b - det > Config::epsilon) {
            t = b - det;
        } else if (b + det > Config::epsilon) {
            t = b + det;
        } else
            return false;
        // puts("SPERE OK3");
        if (t < tMin || t >= h.getT()) return false;
        // puts("SPERE OK4");
        h.set(t, material, (OtoC.squaredLength() <= radius * radius ? -1 : 1) * (r.pointAtParameter(t) - center).normalized());
        return true;
    }

    ObjectBox getObjectBox() override {
        ObjectBox ret;
        ret.x_range = Vector2f(center.x() - radius, center.x() + radius);
        ret.y_range = Vector2f(center.y() - radius, center.y() + radius);
        ret.z_range = Vector2f(center.z() - radius, center.z() + radius);
        return ret;
    }

   private:
    Vector3f center;
    double radius;

   protected:
};

#endif
