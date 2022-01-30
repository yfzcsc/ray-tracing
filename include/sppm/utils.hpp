#ifndef __UTILS__H__
#define __UTILS__H__
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <random>
namespace MyUtils {

static double random01() {
    static std::mt19937 rnd(time(0));
    static std::uniform_real_distribution<> dis01(0.0, 1.0);
    return dis01(rnd);
}

// Quasi-Random Numbers
static int inline rev(int x, int p) { return x == 0 ? 0 : p - x; }
static double randomQMC(const int axis, int x) {
    static int halton[110] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283};
    double ret = 0;
    int p = halton[axis];
    double f = 1. / (double)p, fct = f;
    while (x > 0) {
        ret += rev(x % p, p) * fct;
        x /= p;
        fct *= f;
    }
    return ret;
}

static double sqr(double x) { return x * x; }

static unsigned int hash(const int ix, const int iy, const int iz) { return (unsigned int)((ix * 73856093) ^ (iy * 19349663) ^ (iz * 83492791)) % Config::num_hash; }

static bool bsp_check(const Vector2f& t_range, const Vector2f& z_range, const Vector3f& O, const Vector3f& D, double nx, const double& tMin, double& t, double& x, const std::function<double(double, bool)>& func) {
    if (t_range.x() - Config::epsilon > nx || t_range.y() + Config::epsilon < nx) return false;
    
    if (D.x() == 0 && D.y() == 0) {
        if (fabs(O.x() - func(nx, 0)) < Config::epsilon && fabs(O.y() - func(nx, 1)) < Config::epsilon) {
            if (D.z() != 0) {
                double my_t = (z_range.x() - O.z()) / D.z();
                bool flag = false;
                if(my_t > tMin && t > my_t) t = std::min(t, my_t), flag = true, x = nx;
                my_t = (z_range.y() - O.z()) / D.z();
                if(my_t > tMin && t > my_t) t = std::min(t, my_t), flag = true, x = nx;
                if(!flag) return false;
                    
            } else return false;
            
            return true;
        }
        return false;
    }
    if (fabs(D.x()) > fabs(D.y())) {
        double my_t = (func(nx, 0) - O.x()) / D.x();
        if (my_t < tMin) return false;
        double _tt = my_t * D.z() + O.z();
        if (_tt < z_range.x() - Config::epsilon || _tt > z_range.y() + Config::epsilon) return false;
        // if (fabs(D.y()) > 1e-3) assert(fabs(my_t - (func(nx, 1) - O.y()) / D.y()) > 1e-3);
        if(t > my_t) x = nx, t = my_t;
        return true;
    }
    if (D.y() != 0) {
        double my_t = (func(nx, 1) - O.y()) / D.y();
        if (my_t < tMin) return false;
        double _tt = my_t * D.z() + O.z();
        if (_tt < z_range.x() - Config::epsilon || _tt > z_range.y() + Config::epsilon) return false;
        if(t > my_t) x = nx, t = my_t;
        return true;
    }
    return false;
}

static bool sqr_root(double a, double b, double c, const Vector2f& t_range, const Vector2f& z_range, const Vector3f& O, const Vector3f& D, const double& tMin, double& t, double& x, const std::function<double(double, bool)>& func) {
    if (fabs(a) < Config::epsilon) {
        if (fabs(b) < Config::epsilon) {
            if(fabs(c) > Config::epsilon) return false;
            bool flag = false;
            flag |= bsp_check(t_range, z_range, O, D, t_range.x(), tMin, t, x, func);
            flag |= bsp_check(t_range, z_range, O, D, t_range.y(), tMin, t, x, func);
            if(D.z() != 0){
                double my_t = (z_range.x() - O.z()) / D.z();
                if(my_t > tMin && t > my_t) t = std::min(t, my_t), x = t_range.x(), flag = true;
                my_t = (z_range.y() - O.z()) / D.z();
                if(my_t > tMin && t > my_t) t = std::min(t, my_t), x = t_range.x(), flag = true;    
            }
            return flag;
        } else {
            double _x = -c / b;
            return bsp_check(t_range, z_range, O, D, _x, tMin, t, x, func);
        }
    }
    double dta = b * b - 4 * a * c;
    if (dta < 0) return false;
    dta = sqrt(dta);
    double x1 = (-b - dta) / (2 * a);
    double x2 = (-b + dta) / (2 * a);
    bool flag = false;
    flag |= bsp_check(t_range, z_range, O, D, x1, tMin, t, x, func);
    flag |= bsp_check(t_range, z_range, O, D, x2, tMin, t, x, func);
    return false;
}

static bool cubic_root(double a, double b, double c, double d, const Vector2f& t_range, const Vector2f& z_range, const Vector3f& O, const Vector3f& D, const double& tMin, double& t, double& x, const std::function<double(double, bool)>& func) {
    if (fabs(a) < Config::epsilon) {
        return sqr_root(b, c, d, t_range, z_range, O, D, tMin, t, x, func);
    }
    double p = (3 * a * c - b * b) / (3 * a * a);
    double q = (2 * b * b * b - a * (9 * b * c - 27 * a * d)) / (27 * a * a * a);
    double sx = b / (3 * a);
    bool flag = false;
    if (4 * p * p * p + 27 * q * q < 0) {
        double angle = 1. / 3 * acos(1.5 * q / p * sqrt(-3. / p));
        double p3 = sqrt(-p / 3.);
        double x0 = 2 * p3 * cos(angle) - sx;
        double x1 = 2 * p3 * cos(angle - 2 * Config::pi / 3.) - sx;
        double x2 = 2 * p3 * cos(angle - 4 * Config::pi / 3.) - sx;
        flag |= bsp_check(t_range, z_range, O, D, x0, tMin, t, x, func);
        flag |= bsp_check(t_range, z_range, O, D, x1, tMin, t, x, func);
        flag |= bsp_check(t_range, z_range, O, D, x2, tMin, t, x, func);
    } else {
        double x0;
        if (p < 0) {
            x0 = -2 * fabs(q) / q * sqrt(-p / 3.) * cosh(1. / 3 * acosh(-3 * fabs(q) / (2 * p) * sqrt(-3. / p)));
        } else if (p > 0) {
            x0 = -2 * sqrt(p / 3.) * sinh(1. / 3 * asinh(3 * q / (2 * p) * sqrt(3. / p)));
        } else {
            x0 = -pow(q, 1. / 3);
        }
        x0 -= sx;
        flag |= bsp_check(t_range, z_range, O, D, x0, tMin, t, x, func);
    }
    return flag;
}

static bool bsp_check2(const Vector2f& t_range, double nx, double& x) {
    if (t_range.x() - Config::epsilon > nx || t_range.y() + Config::epsilon < nx) return false;
    x = nx;
    return true;
}

static bool sqr_root2(double a, double b, double c, const Vector2f& t_range, double& x) {
    if (fabs(a) < Config::epsilon) {
        if (fabs(b) < Config::epsilon) {
            return false;
        } else {
            double _x = -c / b;
            return bsp_check2(t_range, _x, x);
        }
    }
    double dta = b * b - 4 * a * c;
    if (dta < 0) return false;
    dta = sqrt(dta);
    double x1 = (-b - dta) / (2 * a);
    double x2 = (-b + dta) / (2 * a);
    bool flag = false;
    flag |= bsp_check2(t_range, x1, x);
    flag |= bsp_check2(t_range, x2, x);
    return true;
}

static bool cubic_root2(double a, double b, double c, double d, const Vector2f& t_range, double& x) {
    if (fabs(a) < Config::epsilon) {
        return sqr_root2(b, c, d, t_range, x);
    }
    double p = (3 * a * c - b * b) / (3 * a * a);
    double q = (2 * b * b * b - a * (9 * b * c - 27 * a * d)) / (27 * a * a * a);
    double sx = b / (3 * a);
    bool flag = false;
    if (4 * p * p * p + 27 * q * q <= 0) {
        double angle = 1. / 3 * acos(1.5 * q / p * sqrt(-3. / p));
        double p3 = sqrt(-p / 3.);
        double x0 = 2 * p3 * cos(angle) - sx;
        double x1 = 2 * p3 * cos(angle - 2 * Config::pi / 3.) - sx;
        double x2 = 2 * p3 * cos(angle - 4 * Config::pi / 3.) - sx;
        flag |= bsp_check2(t_range, x0, x);
        flag |= bsp_check2(t_range, x1, x);
        flag |= bsp_check2(t_range, x2, x);
    } else {
        double x0;
        if (p < 0) {
            x0 = -2 * fabs(q) / q * sqrt(-p / 3.) * cosh(1. / 3 * acosh(-3 * fabs(q) / (2 * p) * sqrt(-3. / p)));
        } else if (p > 0) {
            x0 = -2 * sqrt(p / 3.) * sinh(1. / 3 * asinh(3 * q / (2 * p) * sqrt(3. / p)));
        } else {
            x0 = -pow(q, 1. / 3);
        }
        x0 -= sx;
        flag |= bsp_check2(t_range, x0, x);
    }
    return flag;
}

};  // namespace MyUtils

#endif