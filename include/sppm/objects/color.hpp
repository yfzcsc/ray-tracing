#ifndef __COLOR_H__
#define __COLOR_H__

#include <vecmath.h>

enum ColorType { COLOR = 0, TEXTURE };

class Color {
   public:
    Color() { color = Vector3f::ZERO; }
    explicit Color(const Vector3f& v) : color(v) {}
    explicit Color(const double a, const double b, const double c) : color(Vector3f(a, b, c)) {}
    void set(const Vector3f& v) { color = v; }
    virtual Vector3f getColor(const Vector3f& v) const { return color; }
    Vector3f getColor() const { return color; }
    virtual ColorType type() const { return ColorType::COLOR; };
    virtual void applyMat(const Matrix4f& mat) {}

   private:
    Vector3f color;
};

#endif