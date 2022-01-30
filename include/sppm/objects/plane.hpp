#ifndef PLANE_H
#define PLANE_H

#include <vecmath.h>

#include <cmath>

#include "config.hpp"
#include "hit.hpp"
#include "object.hpp"
#include "ray.hpp"

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object {
   public:
    Plane() = delete;

    Plane(const Vector3f &normal, double d, std::shared_ptr<Material> m, std::shared_ptr<Color> c) : Object(m, c), normal(normal), para_d(d) { _type = ObjectType::THING; }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, double tMin) override {
        if (fabs(Vector3f::dot(r.getDirection(), normal)) <= 1e-9) return false;
        double t = -(-para_d + Vector3f::dot(r.getOrigin(), normal)) / Vector3f::dot(r.getDirection(), normal);
        if (t < tMin || t >= h.getT()) return false;

        h.set(t, material, Vector3f::dot(r.getDirection(), normal) > 0 ? -normal : normal);
        return true;
    }

    ObjectBox getObjectBox() override {
        ObjectBox ret;
        ret.x_range = Vector2f(-Config::inf, Config::inf);
        ret.y_range = Vector2f(-Config::inf, Config::inf);
        ret.z_range = Vector2f(-Config::inf, Config::inf);
        return ret;
    }

    Vector3f getCenter() override { return Vector3f::ZERO; }

   private:
    Vector3f normal;
    double para_d;

   protected:
};

#endif  // PLANE_H
