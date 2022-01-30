#include "bspline.hpp"

#include <fstream>

void BsplineObject::initCal() {
    // We use controls and knots to calculate f, df.
    f.resize(Config::Bdegree + 1);
    df.resize(Config::Bdegree);
    const int K = Config::Bdegree;

    std::vector<double> B[K + 1];
    for (int i = 0; i <= K; ++i) B[i].resize(K + 1);
    for (int i = 0; i <= K; ++i)
        for (int j = 0; j <= K; ++j) B[i][j] = 0.;
    B[K][0] = 1.;
    for (int j = 1; j <= K; j++) {
        for (int k = 0; k <= K; k++) {
            if (knots[k + j] == knots[k]) {
                for (int l = K; l >= 0; --l) B[k][l] = l == 0 ? 0. : B[k][l - 1];
            } else {
                for (int l = K; l >= 0; --l) {
                    double x = l == 0 ? 0. : B[k][l - 1];
                    B[k][l] = (x - knots[k] * B[k][l]) / (knots[k + j] - knots[k]);
                }
            }
            if (k >= K) continue;
            if (knots[k + j + 1] == knots[k + 1]) {
                for (int l = K; l >= 0; --l) {
                    double x = l == 0 ? 0. : B[k + 1][l - 1];
                    B[k][l] += (B[k + 1][l] - x);
                }
            } else {
                for (int l = K; l >= 0; --l) {
                    double x = l == 0 ? 0. : B[k + 1][l - 1];
                    B[k][l] += (knots[k + j + 1] * B[k + 1][l] - x) / (knots[k + j + 1] - knots[k + 1]);
                }
            }
        }
    }

    for (int i = 0; i <= K; ++i) f[i] = Vector3f::ZERO;
    for (int i = 0; i < K; ++i) df[i] = Vector3f::ZERO;
    for (int i = 0; i <= K; ++i) {
        for (int j = 0; j <= K; ++j) {
            f[i] += B[j][i] * controls[j];
        }
    }
    for (int i = 0; i < K; ++i) {
        df[i] = f[i + 1] * (i + 1);
    }
    std::reverse(f.begin(), f.end());
    std::reverse(df.begin(), df.end());

    // printf("[%.6lf]", calY(0.53848705150866882363));

    // for (auto a : controls) printf("[c:%.6lf]", a.x());
    // puts("");
    // for (int i = 0; i <= 10; ++i) printf("[%.6lf:%.6lf]", i / 10. * (t_range.y() - t_range.x()) + t_range.x(), caldX(i / 10. * (t_range.y() - t_range.x()) + t_range.x()));
}

ObjectBox BsplineObject::getObjectBox() {
    ObjectBox ret;
    ret.z_range = z_range;
    ret.x_range = Vector2f(controls[0].x(), controls[0].x());
    ret.y_range = Vector2f(controls[0].y(), controls[0].y());
    for (auto& c : controls) {
        ret.x_range = Vector2f(std::min(ret.x_range.x(), c.x()), std::max(ret.x_range.y(), c.x()));
        ret.y_range = Vector2f(std::min(ret.y_range.x(), c.y()), std::max(ret.y_range.y(), c.y()));
    }
    return ret;
}

bool BsplineObject::intersect(const Ray& ray, Hit& hit, double tMin) {
    // Ox+Dx*t=fx(p)
    // Oy+Dy*t=fy(p)
    const int K = Config::Bdegree;
    std::vector<double> nf;
    nf.resize(K + 1);

    const auto& O = ray.getOrigin();
    const auto& D = ray.getDirection();

    for (int i = 0; i <= K; ++i) {
        nf[i] = f[i].x() * D.y() - f[i].y() * D.x();
        if (i == K) nf[i] = nf[i] - (O.x() * D.y() - O.y() * D.x());
    }

    double t_sol = 1e40, x_sol;
    bool flag = MyUtils::cubic_root(nf[0], nf[1], nf[2], nf[3], t_range, z_range, O, D, tMin, t_sol, x_sol, [this](double theta, bool flag) { return flag ? this->calY(theta) : this->calX(theta); });
    if (!flag) return false;
    // puts("flag ok");

    if (t_sol >= hit.getT()) return false;

    // puts("Hit /BSP!");

    // printf("[calX:%.6lf, calY:%.6lf]\n", calX(x_sol), calY(x_sol));
    // printf("[rlX:%.6lf, rlY:%.6lf]\n", ray.pointAtParameter(t_sol).x(), ray.pointAtParameter(t_sol).y());

    Vector3f vec_n = Vector3f::cross(Vector3f(caldX(x_sol), caldY(x_sol), 0.).normalized(), Vector3f(0., 0., 1.));

    if (Vector3f::dot(vec_n, D) > 0) vec_n.negate();
    hit.set(t_sol, material, vec_n);
    // printf("vec_n:");
    // vec_n.print();
    return true;
}

void BsplineGroup::load(std::string filename, Vector2f z_range) {
    std::ifstream in(filename);
    int n;

    in >> n;
    controls.resize(n);
    knots.resize(n + Config::Bdegree + 1);
    for (int i = 0; i < n; ++i) {
        double x, y, z, w;
        in >> x >> y >> z >> w;
        controls[i] = Vector3f(x, y, z);
    }

    for (int i = 0; i < n + Config::Bdegree + 1; ++i) {
        double knot;
        in >> knot;
        knots[i] = knot;
    }

    in.close();

    group.clear();

    Vector3f cen = calcCenter();

    for(auto& c : controls) c = c - cen;

    for (int i = Config::Bdegree; i < n; ++i) {
        const auto& C = std::vector<Vector3f>(controls.begin() + i - (Config::Bdegree), controls.begin() + i + 1);
        const auto& K = std::vector<double>(knots.begin() + i - (Config::Bdegree), knots.begin() + i + 2 + Config::Bdegree);
        std::shared_ptr<BsplineObject> O = std::make_shared<BsplineObject>(C, K, Vector2f(knots[i], knots[i + 1]), z_range, material, color);
        group.push_back(O);
    }
}

void BsplineGroup::loadring(std::string filename, Vector2f z_range, Vector3f center, double alpha) {
    std::ifstream in(filename);
    int n;

    in >> n;
    controls.resize(n);
    knots.resize(n + Config::Bdegree + 1);
    for (int i = 0; i < n; ++i) {
        double x, y, z, w;
        in >> x >> y >> z >> w;
        controls[i] = Vector3f(x, y, z);
    }

    for (int i = 0; i < n + Config::Bdegree + 1; ++i) {
        double knot;
        in >> knot;
        knots[i] = knot;
    }

    in.close();

    group.clear();

    Vector3f cen = calcCenter();

    for(auto& c : controls) c = c - cen;

    for (int i = Config::Bdegree; i < n; ++i) {
        const auto& C = std::vector<Vector3f>(controls.begin() + i - (Config::Bdegree), controls.begin() + i + 1);
        const auto& K = std::vector<double>(knots.begin() + i - (Config::Bdegree), knots.begin() + i + 2 + Config::Bdegree);
        std::shared_ptr<BsplineRing> O = std::make_shared<BsplineRing>(center, alpha, C, K, Vector2f(knots[i], knots[i + 1]), z_range, material, color);
        group.push_back(O);
        // break;
    }
}

ObjectBox BsplineRing::getObjectBox() {
    ObjectBox ret;
    ret.z_range = Vector2f(center.z(), center.z());
    ret.x_range = Vector2f(controls[0].x(), controls[0].x()) + Vector2f(center.x(), center.x());
    ret.y_range = Vector2f(controls[0].y(), controls[0].y()) + Vector2f(center.y(), center.y());
    for (auto& c : controls) {
        ret.x_range = Vector2f(std::min(ret.x_range.x(), c.x() + center.x()), std::max(ret.x_range.y(), c.x() + center.x()));
        ret.y_range = Vector2f(std::min(ret.y_range.x(), c.y() + center.y()), std::max(ret.y_range.y(), c.y() + center.y()));
        ret.x_range = Vector2f(std::min(ret.x_range.x(), c.x() * alpha + center.x()), std::max(ret.x_range.y(), c.x() * alpha + center.x()));
        ret.y_range = Vector2f(std::min(ret.y_range.x(), c.y() * alpha + center.y()), std::max(ret.y_range.y(), c.y() * alpha + center.y()));
    }
    return ret;
}

bool BsplineRing::intersect(const Ray& ray, Hit& hit, double tMin) {

    const auto& O = ray.getOrigin();
    const auto& D = ray.getDirection();
    const int K = Config::Bdegree;

    if(D.z() == 0 && fabs(O.z() - center.z()) < Config::epsilon) return false;

    const auto Dx = D / D.z();
    const auto Ox = O + Dx * (center.z() - O.z()) - center;

    double t_sol = (center.z() - O.z()) / D.z();

    

    if(t_sol < tMin || t_sol >= hit.getT()) return false;

    std::vector<double> nf;
    nf.resize(K + 1);


    for (int i = 0; i <= K; ++i) {
        nf[i] = Ox.x() * f[i].y() - Ox.y() * f[i].x();
    }

    double x_sol = 1e40;
    bool flag = MyUtils::cubic_root2(nf[0], nf[1], nf[2], nf[3], t_range, x_sol);
    if (!flag) return false;

    double fx = calX(x_sol);
    double fy = calY(x_sol);

    double beta = 1e40;

    if (fabs(fx) > fabs(fy)) {
        beta = Ox.x() / fx;
    } else if (fy != 0) {
        beta = Ox.y() / fy;
    } else return false;

    // if(fx != 0 && fy != 0)
    //     assert(fabs((Ox.x() - center.x()) / fx - (Ox.y() - center.y()) / fy) < Config::epsilon);


    if(beta <= 1 + Config::epsilon && beta > alpha - Config::epsilon) {
        Vector3f vec_n = Vector3f(0, 0, 1);
        if (Vector3f::dot(vec_n, D) > 0) vec_n.negate();

        // printf("[Ok, %.6lf, %.6lf, %.6lf, %.6lf, %.6lf(%.6lf, %.6lf)]\n", beta, fx, fy, fx + center.x(), fy + center.y(), t_range.x(), t_range.y());

        // printf("<x_sol=%.20lf>", x_sol);

        // O.print();
        
        // D.print();

        // printf("[tsol=%.6lf]\n", t_sol);

        hit.set(t_sol, material, vec_n);
        return true;
    }

    return false;
}