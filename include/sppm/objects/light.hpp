#ifndef LIGHT_H
#define LIGHT_H

#include <vecmath.h>

#include "hit.hpp"
#include "object.hpp"
#include "utils.hpp"

class Light : public Object {
   public:
    Light() = delete;
    Light(std::shared_ptr<Color> c, const Vector3f& iw) {
        _type = ObjectType::LIGHT;
        color = c;
        init_weight = iw;
        material = std::make_shared<Material>(Materials[MaterialType::MLIGHT]);
    }

    virtual Ray genRandomRay(int id) = 0;

    Vector3f _color() { return color->getColor(); }

    Vector3f _weight() { return init_weight; }

   protected:
    Vector3f init_weight;
};

class CircleLight : public Light {
   public:
    CircleLight() = delete;
    CircleLight(std::shared_ptr<Color> c, const Vector3f& iw, const Vector3f& center, const double r, const Vector3f& normal) : Light(c, iw) {
        this->center = center;
        this->R = r;
        this->normal = normal;
        this->para_d = -Vector3f::dot(center, normal);
    }

    ObjectBox getObjectBox() {
        ObjectBox ret;
        ret.x_range = Vector2f(-Config::inf, Config::inf);
        ret.y_range = Vector2f(-Config::inf, Config::inf);
        ret.z_range = Vector2f(-Config::inf, Config::inf);
        return ret;
    }

    Vector3f getCenter() { return center; }

    bool intersect(const Ray& r, Hit& h, double tMin) override {
        if (fabs(Vector3f::dot(r.getDirection(), normal)) <= 1e-9) return false;
        double t = -(-para_d + Vector3f::dot(r.getOrigin(), normal)) / Vector3f::dot(r.getDirection(), normal);
        if (t < tMin || t >= h.getT()) return false;

        Vector3f pos = r.pointAtParameter(t);
        if ((pos - center).squaredLength() > R * R) return false;

        h.set(t, material, Vector3f::dot(r.getDirection(), normal) > 0 ? -normal : normal);
        return true;
    }

    Ray genRandomRay(int id) override {
        double p = 2. * Config::pi * MyUtils::randomQMC(0, id), t = 2. * acos(sqrt(1. - MyUtils::randomQMC(1, id)));
        double st = sin(t);
        return Ray(center, Vector3f(cos(p) * st, cos(t), sin(p) * st));
    }

   private:
    Vector3f center;
    Vector3f vec_r;
    Vector3f normal;
    double para_d;
    double R;
};

#endif  // LIGHT_H
