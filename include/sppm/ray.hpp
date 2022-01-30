#ifndef RAY_H
#define RAY_H

#include <vecmath.h>

#include <cassert>
#include <iostream>

#include "config.hpp"
#include "utils.hpp"

// Ray class copied from PA1
// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray {
   public:
    Ray() {}
    Ray(const Vector3f &orig, const Vector3f &dir) {
        origin = orig;
        direction = dir;
    }

    Ray(const Ray &r) {
        origin = r.origin;
        direction = r.direction;
    }

    const Vector3f &getOrigin() const { return origin; }

    const Vector3f &getDirection() const { return direction; }

    Vector3f pointAtParameter(double t) const { return origin + direction * t; }

    Ray specular(const Vector3f &hitPoint, const Vector3f &hitNormal) const {
        auto dir = direction - 2 * Vector3f::dot(hitNormal, direction) * hitNormal;
        return Ray(hitPoint + Config::epsilon * dir, dir);
    }

    Ray diffuse(const Vector3f &hitPoint, const Vector3f &hitNormal, int depth, long long id, double s) const {
        Vector3f u = Vector3f::cross(Vector3f::UP, hitNormal);
        if (u.squaredLength() <= Config::epsilon) u = Vector3f::cross(Vector3f::RIGHT, hitNormal);
        Vector3f v = Vector3f::cross(hitNormal, u);
        double theta = MyUtils::randomQMC(depth * 2 + 1, id) * 2 * Config::pi;
        double phi = MyUtils::randomQMC(depth * 2 + 2, id);
        Vector3f dir = hitNormal * sqrt(1 - phi) + (u * cos(theta) + v * sin(theta)) * sqrt(phi);
        return Ray(hitPoint + Config::epsilon * dir, dir);
    }

    Ray smallStep(const Vector3f &hitPoint) const { return Ray(hitPoint + Config::epsilon * direction, direction); }

    Ray refraction(const Vector3f &hitPoint, const Vector3f &hitNormal, double refIndex, double thetaIn, double thetaOut) const {
        Vector3f dr = (direction * refIndex + hitNormal * (thetaIn * refIndex - thetaOut)).normalized();
        return Ray(hitPoint + Config::epsilon * dr, dr);
    }

   private:
    Vector3f origin;
    Vector3f direction;
};

inline std::ostream &operator<<(std::ostream &os, const Ray &r) {
    os << "Ray <" << r.getOrigin() << ", " << r.getDirection() << ">";
    return os;
}

#endif  // RAY_H
