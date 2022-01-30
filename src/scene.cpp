#include "scene.hpp"

#include <algorithm>
#include <cassert>

#include "config.hpp"
#include "utils.hpp"

void Scene::trace(const Ray& r, const Vector3f& flux, const Vector3f& weight, int depth, int id, TraceType type, SPPMHitPoint* hp, int incoming) {
    // if (depth > Config::depth_limit && type == TraceType::EYE) {
    //     printf("FUCK\n");
    // }
    if (depth > Config::depth_limit || (type == TraceType::EYE && weight.max_xyz() < 1e-7) ) return;
    Hit hit_point;
    std::shared_ptr<Object> O = objects.intersect(r, hit_point, Config::tMin);
    // printf("[flag=%d]", O.get() == nullptr);
    if (O.get() == nullptr) return;
    // printf("[final t = %.6lf]\n",hit_point.getT());
    Vector3f point = r.pointAtParameter(hit_point.getT());

    // if (type == TraceType::PHOTON)
    //     printf("trace(%d)[(%.3lf)%.3lf, %.3lf,%.3lf(%.3lf, %.3lf, %.3lf)]->\n", depth, weight.max_xyz(), r.getOrigin().x(), r.getOrigin().y(), r.getOrigin().z(), r.getDirection().x(), r.getDirection().y(), r.getDirection().z());
    // else
    //     printf("traceEye(%d)[(%.3lf)%.3lf, %.3lf,%.3lf(%.3lf, %.3lf, %.3lf)]->\n", depth, weight.max_xyz(), r.getOrigin().x(), r.getOrigin().y(), r.getOrigin().z(), r.getDirection().x(), r.getDirection().y(), r.getDirection().z());

    // point.print();
    // printf("[t=%.6lf, %.6lf, %.6lf, %.6lf]\n normal:", hit_point.getT(), hit_point.specular(), hit_point.diffuse(), hit_point.refraction());
    // hit_point.getNormal().print();
    double total_light = hit_point.total_light();
    double action = MyUtils::random01() * total_light;
    if (action <= hit_point.specular() && hit_point.specular() > 0) {
        // puts("B");
        trace(r.specular(point, hit_point.getNormal()), O->getColor(point) * flux, O->getColor(point) * weight, depth + 1, id, type, hp, incoming);
        return;
    }

    action -= hit_point.specular();
    if (action <= hit_point.diffuse() && hit_point.diffuse() > 0) {
        // puts("C");
        if (type == TraceType::EYE) {
#pragma omp critical
            {
                real_sppmhits.emplace_back();
                RealSPPMHitPoint* rhp = &real_sppmhits.back();
                rhp->init(first_init_rad);
                rhp->lastWeight = O->getColor(point) * weight * total_light;
                // if (hp->lastWeight.squaredLength() <= 1e-8) {
                // printf("[%.6lf, %.6lf, %.6lf]\n", O->getColor().squaredLength(), weight.squaredLength(), total_light);
                // }
                rhp->point = point;
                rhp->normal = hit_point.getNormal();
                rhp->dir = r.getDirection();
                rhp->m = hit_point.getMaterial();
                rhp->hp = hp;
                // point.print();
                if (O->type() == ObjectType::LIGHT) {
                    hp->fluxLight += rhp->lastWeight;
                    rhp->valid = false;
                } else {
                    rhp->valid = true;
                }
            }
        } else {
            updateHit(flux, point, r.getDirection());
            double action2 = MyUtils::random01();
            if (action2 <= hit_point.Kd().max_xyz()) {
                trace(r.diffuse(point, hit_point.getNormal(), depth, id, hit_point.phong_s()), flux * O->getColor(point) * total_light, weight, depth + 1, id, type, hp, incoming);
            }
        }
        return;
    }

    action -= hit_point.diffuse();
    if (action <= hit_point.refraction() && hit_point.refraction() > 0) {
        // puts("A");
        double refIndex = incoming ? 1. / hit_point.refractiveIndex() : hit_point.refractiveIndex();
        double ddn = Vector3f::dot(hit_point.getNormal(), r.getDirection());
        double cos2t = 1 - refIndex * refIndex * (1 - ddn * ddn);
        Vector3f td = (r.getDirection() * refIndex - hit_point.getNormal() * (ddn * refIndex + sqrt(cos2t))).normalized();
        double R0 = MyUtils::sqr((1 - refIndex) / (1 + refIndex));
        double Re = R0 + (1 - R0) * pow(1 - (incoming ? -ddn : Vector3f::dot(td, -hit_point.getNormal())), 5);
        if (cos2t < 0) {
            // printf("[spe]");
            trace(r.specular(point, (incoming ? 1 : -1) * hit_point.getNormal()), flux, weight, depth + 1, id, type, hp, incoming);

        } else if(type == TraceType::EYE){
            trace(Ray(point, td).smallStep(point), flux, weight * O->getColor(point) * total_light * (1 - Re), depth + 1, id, type, hp, incoming ^ 1);
            trace(r.specular(point, (incoming ? 1 : -1) * hit_point.getNormal()), flux, weight * O->getColor(point) * total_light * Re, depth + 1, id, type, hp, incoming);
        } else {
            if (MyUtils::random01() < Re) {
                // printf("[spe]");
                trace(r.specular(point, (incoming ? 1 : -1) * hit_point.getNormal()), flux, weight * O->getColor(point) * total_light, depth + 1, id, type, hp, incoming);

            } else {
                // printf("[re,%.3lf]", (point - Vector3f(40, 50, 10)).squaredLength());
                trace(Ray(point, td).smallStep(point), flux, weight * O->getColor(point) * total_light, depth + 1, id, type, hp, incoming ^ 1);
            }
        }
        return;
    }
    // fprintf(stderr, "[%.6lf,%.6lf]", action, hit_point.refraction());
    // exit(0);
}
/*
void Scene::build(int& rt, int l, int r, int d) {
    if (!rt) {
        rt = ++szKDTree;
        sppmhitTree[rt].clr();
    }
    SPPMHitKDTreePoint& s = sppmhitTree[rt];
    if (l == r) {
        s.data = sppmhitIndex[l];
        s.maxr2 = s.data->r2;
        s.range.init(s.data->point);
        return;
    }
    int mid = (l + r) / 2;
    if (d == 0) {
        std::nth_element(sppmhitIndex.begin() + l, sppmhitIndex.begin() + mid, sppmhitIndex.begin() + r + 1, [&](const SPPMHitPoint* a, const SPPMHitPoint* b) { return a->point.x() < b->point.x(); });
    } else if (d == 1) {
        std::nth_element(sppmhitIndex.begin() + l, sppmhitIndex.begin() + mid, sppmhitIndex.begin() + r + 1, [&](const SPPMHitPoint* a, const SPPMHitPoint* b) { return a->point.y() < b->point.y(); });
    } else if (d == 2) {
        std::nth_element(sppmhitIndex.begin() + l, sppmhitIndex.begin() + mid, sppmhitIndex.begin() + r + 1, [&](const SPPMHitPoint* a, const SPPMHitPoint* b) { return a->point.z() < b->point.z(); });
    }
    d = (d == 2 ? 0 : d + 1);
    build(s.lc, l, mid, d);
    build(s.rc, mid + 1, r, d);
    s.range = sppmhitTree[s.lc].range + sppmhitTree[s.rc].range;
    s.maxr2 = std::max(sppmhitTree[s.lc].maxr2, sppmhitTree[s.rc].maxr2);
}

void Scene::buildHitKDTree() {
    rtKDTree = 0;
    szKDTree = 0;
    sppmhitTree[0].clr();
    build(rtKDTree, 0, width * height - 1, 0);
}

void Scene::update(int& rt, const Vector3f& weight, const Vector3f& point, const Vector3f& dir, int d) {
    if (!rt) return;
    SPPMHitKDTreePoint& s = sppmhitTree[rt];
    if (!s.range.include(point, s.maxr2)) return;
    if (s.data) {
        if (!s.data->valid) return;
        CNT++;
        SPPMHitPoint* hp = s.data;
        double factor = (hp->n * Config::alpha + Config::alpha) / (hp->n * Config::alpha + 1.);
        double rho = hp->m->cal(dir, hp->normal, hp->dir);
        hp->n++;
        hp->r2 *= factor;
        s.maxr2 = hp->r2;
        hp->flux = (hp->flux + hp->lastWeight * weight * rho * (1. / Config::pi)) * factor;
        // if (hp->flux.squaredLength() <= 1e-8 && hp->n >= 30) printf("[%d, %.6lf(%.6lf, %.6lf, %.6lf)]\n", hp->n, hp->flux.x(), rho, hp->lastWeight.x(), weight.x());
        // point.print();
        // hp->point.print();
        return;
    }
    update(s.lc, weight, point, dir, d + 1);
    update(s.rc, weight, point, dir, d + 1);
    s.maxr2 = std::max(sppmhitTree[s.lc].maxr2, sppmhitTree[s.rc].maxr2);
}
*/

void Scene::updateHit(const Vector3f& weight, const Vector3f& point, const Vector3f& dir) {
    Vector3f hh = (point - minVec) / (init_rad * 2.0);
    int ix = abs(int(hh.x())), iy = abs(int(hh.y())), iz = abs(int(hh.z()));
    int hpos = MyUtils::hash(ix, iy, iz);
    for (int t = headHashGrid[hpos]; t != -1; t = dataHashGrid[t].nxt) {
        // printf("[%d]", t);
        // fflush(stdout);
        auto rhp = dataHashGrid[t].data;
        if (!rhp->valid || !rhp->check(point)) continue;
        CNT++;
        double factor = (rhp->n * Config::alpha + Config::alpha) / (rhp->n * Config::alpha + 1.);
        Vector3f rho = rhp->m->cal(dir, rhp->normal, rhp->dir);
        rhp->n++;
        rhp->r2 *= factor;
        rhp->flux = (rhp->flux + rhp->lastWeight * weight * rho * (1. / Config::pi)) * factor;
    }
}

Ray Scene::genRandomRay(int id) { return source->genRandomRay(id); }

void Scene::addObject(std::shared_ptr<Object> a) {
    objects.addObject(a);
    // assert(a->material.get() != nullptr);
}

void Scene::addLight(std::shared_ptr<Light> a) {
    objects.addObject(a);
    source = a;
}

void Scene::addMesh(std::shared_ptr<Mesh> m) {
    for (int i = 0; i < m->triangles.size(); ++i) {
        // puts("OK");
        // m->triangles[i].print();
        addObject(std::make_shared<MeshTriangle>(m->triangles[i]));
    }
}

void Scene::addBsplineGroup(std::shared_ptr<BsplineGroup> m) {
    for (int i = 0; i < m->group.size(); ++i) {
        // puts("OK");
        // m->triangles[i].print();
        addObject(m->group[i]);
    }
}

Vector3f Scene::getFlux(int x, int numR, int numP) {
    SPPMHitPoint& hp = sppmhit[x];
    /*
        if (hp.n == 0 || (hp.flux / (Config::pi * hp.r2 * numP * numR) + source->_color() * hp.fluxLight / numR).squaredLength() <= 1e-6) {
            printf("<%d>", x);
            printf("point[x = %.6lf, y = %.6lf, z = %.6lf]\n", hp.point.x(), hp.point.y(), hp.point.z());
            printf("[n = %d, r2 = %.6lf, fL = %.6lf, flux = %.6lf]\n", hp.n, hp.r2, hp.fluxLight.x(), hp.flux.x());
        }*/
    // auto ret = hp.flux / (Config::pi * hp.r2 * numP * numR) + source->_color() * hp.fluxLight / numR;
    auto ret = hp.flux / (numP * numR) + source->_color() * hp.fluxLight / numR;
    return ret;
}

void Scene::updateAllFlux() {
    for(auto a : real_sppmhits) {
        assert(a.hp != nullptr);
        a.hp->flux += a.flux / (Config::pi * a.r2);
    }
}

void Scene::buildHashGrid() {
    for (int i = 0; i < Config::num_hash; ++i) headHashGrid[i] = -1, szHashGrid[i] = 0;
    printf("<hit points: %d>\n", real_sppmhits.size());
    minVec = real_sppmhits[0].point;
    init_rad = real_sppmhits[0].r2;
    for (int i = 0; i < real_sppmhits.size(); ++i) {
        init_rad = std::max(init_rad, real_sppmhits[i].r2);
        for (int j = 0; j < 3; ++j) minVec[j] = std::min(minVec[j], real_sppmhits[i].point[j]);
    }
    init_rad = sqrt(init_rad) + Config::epsilon;
    Vector3f irad(init_rad, init_rad, init_rad);
    minVec -= irad;
    double hash_s = 1.0 / (init_rad * 2.0);
    int szData = 1;
    for (int i = 0; i < real_sppmhits.size(); ++i) {
        Vector3f BMin = ((real_sppmhits[i].point - irad) - minVec) * hash_s;
        Vector3f BMax = ((real_sppmhits[i].point + irad) - minVec) * hash_s;
        for (int iz = abs(int(BMin.z())); iz <= abs(int(BMax.z())); iz++)
            for (int iy = abs(int(BMin.y())); iy <= abs(int(BMax.y())); iy++)
                for (int ix = abs(int(BMin.x())); ix <= abs(int(BMax.x())); ix++) {
                    int hv = MyUtils::hash(ix, iy, iz);
                    ++szData;
                    if (dataHashGrid.size() <= szData) {
                        dataHashGrid.resize(szData + 100);
                    }
                    dataHashGrid[szData].nxt = headHashGrid[hv];
                    dataHashGrid[szData].data = &real_sppmhits[i];
                    headHashGrid[hv] = szData;
                    szHashGrid[hv]++;
                }
    }
}