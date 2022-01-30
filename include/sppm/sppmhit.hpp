#ifndef _SPPM_HIT_H__
#define _SPPM_HIT_H__

#include <vecmath.h>

#include "config.hpp"
#include "material.hpp"
#include "object.hpp"

class SPPMHitPoint {
   public:
    Vector3f flux;
    Vector3f fluxLight;
    void init() {
        flux = fluxLight = Vector3f::ZERO;
    }
};

class RealSPPMHitPoint {
   public:
    Vector3f point;
    Vector3f normal;
    Vector3f dir;
    double r2;
    Vector3f lastWeight;
    Vector3f flux;
    Vector3f fluxLight;
    bool valid;
    int n;
    std::shared_ptr<Material> m;
    SPPMHitPoint* hp;
    void init(double _r2) {
        r2 = _r2;
        dir = point = normal = Vector3f::ZERO;
        lastWeight = flux = fluxLight = Vector3f::ZERO;
        valid = false;
        n = 0;
        hp = nullptr;
        m = nullptr;
    }
    bool check(Vector3f P) { return (P - point).squaredLength() <= r2; }
};

class SPPMHitKDTreePoint {
   public:
    int lc, rc;
    double maxr2;
    ObjectBox range;
    SPPMHitPoint* data;
    void clr() {
        lc = rc = 0;
        maxr2 = 0.;
        data = nullptr;
        range.clr();
    }
};

#endif