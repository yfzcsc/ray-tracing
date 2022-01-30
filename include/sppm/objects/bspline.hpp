#ifndef _BEZIER_H__
#define _BEZIER_H__

#include "object.hpp"

class BsplineObject : public Object {
   public:
    BsplineObject() = delete;
    BsplineObject(std::vector<Vector3f> con, std::vector<double> knots, Vector2f t_range, Vector2f z_range, std::shared_ptr<Material> m, std::shared_ptr<Color> c) : Object(m, c), t_range(t_range), z_range(z_range), controls(con), knots(knots) {}
    void initCal();
    double calX(double t) {
        double ret = 0;
        for (auto a : f) ret = ret * t + a.x();
        return ret;
    }
    double calY(double t) {
        double ret = 0;
        for (auto a : f) ret = ret * t + a.y();
        return ret;
    }
    double caldX(double t) {
        double ret = 0;
        for (auto a : df) ret = ret * t + a.x();
        return ret;
    }
    double caldY(double t) {
        double ret = 0;
        for (auto a : df) ret = ret * t + a.y();
        return ret;
    }

    bool intersect(const Ray& r, Hit& h, double tMin) override;

    ObjectBox getObjectBox() override;

    void translateZ(double z_trans) { z_range += Vector2f(z_trans, z_trans); }
    void transformXY(const Matrix3f& mat) {
        for (auto& a : controls) {
            Vector2f v = (mat * Vector3f(a.xy(), 1)).xy();
            a = Vector3f(v, 0.);
        }
    }

   protected:
    Vector2f t_range;
    std::vector<Vector3f> f;
    std::vector<Vector3f> df;
    std::vector<Vector3f> controls;
    std::vector<double> knots;
   private:
    Vector2f z_range;
};

class BsplineRing : public BsplineObject {
   public:
    BsplineRing() = delete;
    BsplineRing(Vector3f center, double alpha, std::vector<Vector3f> con, std::vector<double> knots, Vector2f t_range, Vector2f z_range, std::shared_ptr<Material> m, std::shared_ptr<Color> c):
        BsplineObject(con, knots, t_range, z_range, m, c), center(center), alpha(alpha){}

    bool intersect(const Ray& r, Hit& h, double tMin) override;

    ObjectBox getObjectBox() override;

   private:
    Vector3f center;
    double alpha;
};

class BsplineGroup {
   public:
    BsplineGroup() {}
    BsplineGroup(std::string filename, Vector2f z_range, std::shared_ptr<Material> m, std::shared_ptr<Color> c) : material(m), color(c), z_range(z_range) { load(filename, z_range); }
    BsplineGroup(Vector3f center, double alpha, std::string filename, Vector2f z_range, std::shared_ptr<Material> m, std::shared_ptr<Color> c) : material(m), color(c), z_range(z_range) { loadring(filename, z_range, center, alpha); }
    void load(std::string filename, Vector2f z_range);
    void loadring(std::string filename, Vector2f z_range, Vector3f center, double alpha);

    void translateZ(double z_trans) {
        z_range += Vector2f(z_trans, z_trans);
        for (auto& o : group) o->translateZ(z_trans);
    }
    void transformXY(const Matrix3f& mat) {
        // for (auto& a : controls) a.print();
        // puts("------");
        for (auto& a : controls) {
            Vector2f v = (mat * Vector3f((a).xy(), 1)).xy();
            a = Vector3f(v, 0.);
            // a.print();
        }
        for (auto& o : group) o->transformXY(mat);
    }

    Vector3f calcCenter() {

        Vector3f ret = Vector3f::ZERO;
        // return ret;
        double sum = 0;
        for (int i = Config::Bdegree; i < controls.size(); ++i) {
            ret += controls[i];
            sum += 1;
        }
        (ret / sum).print();
        return ret / sum;
    }

    void transCenterTo(const Vector3f& v, double sXY) {
        Vector3f cen = calcCenter();
        Vector3f vp = v - cen;
        transformXY(Matrix3f(Vector3f(sXY, 0, vp.x() - cen.x() * sXY + cen.x()), Vector3f(0, sXY, vp.y() - cen.y() * sXY + cen.y()), Vector3f(0, 0, 0), false));
        translateZ(v.z() - z_range.x());
    }

    void scaleXY(double sXY) {
        Vector3f cen = calcCenter();
        transformXY(Matrix3f(Vector3f(sXY, 0, -cen.x() * sXY + cen.x()), Vector3f(0, sXY, -cen.y() * sXY + cen.y()), Vector3f(0, 0, 0), false));
    }

    void initCal() {
        for (auto& o : group) o->initCal();
    }

    Vector2f z_range;
    std::shared_ptr<Material> material;
    std::shared_ptr<Color> color;
    std::vector<Vector3f> controls;
    std::vector<double> knots;
    std::vector<std::shared_ptr<BsplineObject>> group;
};

#endif