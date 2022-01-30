#ifndef CAMERA_H
#define CAMERA_H

#include <vecmath.h>

#include <cmath>

#include "ray.hpp"

class Camera {
   public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up);
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

   protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {
   public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH, double angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        fx = tan(angle / 2.) / (imgW / 2.);
        fy = tan(angle / 2.) / (imgH / 2.);
    }

    Ray generateRay(const Vector2f &point) override {
        Vector3f dir((point.x() - width / 2.) * fx, (point.y() - height / 2.) * fy, 1);
        dir = (Matrix3f(horizontal, up, direction) * dir + Vector3f(Config::epsilon, Config::epsilon, Config::epsilon)).normalized();
        return Ray(center, dir);
    }

   private:
    double fx, fy;
};

class DepthFieldCamera : public Camera {
   public:
    DepthFieldCamera(const Vector3f &lookfrom, const Vector3f &lookat, const Vector3f &up, int imgW, int imgH, double angle, double radius) : Camera(lookfrom, (lookat - lookfrom).normalized(), up, imgW, imgH) {
        // angle is in radian.
        fx = tan(angle / 2.) / (imgW / 2.);
        fy = tan(angle / 2.) / (imgH / 2.);
        this->radius = radius;
        this->lookat = lookat;
        this->lookfrom = lookfrom;
    }

    Ray generateRay(const Vector2f &point) override {

        double R = MyUtils::random01() * radius;
        double A = MyUtils::random01() * 2 * Config::pi;
        Vector3f offset(R * cos(A), R * sin(A), 0);

        Vector3f dir((point.x() - width / 2.) * fx, (point.y() - height / 2.) * fy, 1);
        dir = (Matrix3f(horizontal, up, direction) * dir + Vector3f(Config::epsilon, Config::epsilon, Config::epsilon)).normalized();

        double focus = (lookat - lookfrom).length();

        return Ray(lookfrom + offset, (focus * dir - offset).normalized());
    }

   private:
    double fx, fy, radius;
    Vector3f lookat, lookfrom;
};

#endif  // CAMERA_H
