#ifndef __OBJECT__H__
#define __OBJECT__H__

#include <memory>

#include "color.hpp"
#include "hit.hpp"
#include "material.hpp"
#include "utils.hpp"
#include "vecmath.h"

enum ObjectType { THING = 0, LIGHT };

class ObjectBox {
   public:
    Vector2f x_range;
    Vector2f y_range;
    Vector2f z_range;
    void clr() { x_range = y_range = z_range = Vector2f(Config::inf, -Config::inf); }
    ObjectBox operator+(const ObjectBox &o) const {
        ObjectBox ret;
        ret.x_range = Vector2f(std::min(x_range.x(), o.x_range.x()), std::max(x_range.y(), o.x_range.y()));
        ret.y_range = Vector2f(std::min(y_range.x(), o.y_range.x()), std::max(y_range.y(), o.y_range.y()));
        ret.z_range = Vector2f(std::min(z_range.x(), o.z_range.x()), std::max(z_range.y(), o.z_range.y()));
        return ret;
    }
    bool include(const Vector3f &point) const { return point.x() >= x_range.x() - Config::epsilon && point.x() <= x_range.y() + Config::epsilon && point.y() >= y_range.x() - Config::epsilon && point.y() <= y_range.y() + Config::epsilon && point.z() >= z_range.x() - Config::epsilon && point.z() <= z_range.y() + Config::epsilon; }
    bool include(const Vector3f &point, double r2) const {
        double sum = getNear(point.x(), x_range.x(), x_range.y());
        sum += getNear(point.y(), y_range.x(), y_range.y());
        sum += getNear(point.z(), z_range.x(), z_range.y());
        return sum <= r2;
    }
    void init(Vector3f x) {
        x_range = Vector2f(x.x(), x.x());
        y_range = Vector2f(x.y(), x.y());
        z_range = Vector2f(x.z(), x.z());
    }

   private:
    double getNear(double x, double L, double R) const {
        if (x > R)
            return MyUtils::sqr(x - R);
        else if (x < L)
            return MyUtils::sqr(L - x);
        return 0;
    }
};

class Object {
   public:
    Object() : material(nullptr), color(nullptr) {}
    virtual ~Object() = default;
    explicit Object(std::shared_ptr<Material> m, std::shared_ptr<Color> c) : material(m), color(c) {}

    virtual ObjectBox getObjectBox() = 0;
    virtual bool intersect(const Ray &r, Hit &h, double tMin) = 0;

    void setColor(const Vector3f &v) { color = std::make_shared<Color>(v); }
    void setMaterial(const Material &m) { material = std::make_shared<Material>(m); }
    Vector3f getColor(const Vector3f &point) const { return color->getColor(point); }
    std::shared_ptr<Material> getMaterial() { return material; }

    ObjectType type() { return _type; }

   protected:
    std::shared_ptr<Material> material;
    std::shared_ptr<Color> color;
    ObjectType _type;
};

#endif
