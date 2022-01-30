#ifndef __SCENE_H___
#define __SCENE_H___

#include <vector>

#include "light.hpp"
#include "mesh.hpp"
#include "objects/bspline.hpp"
#include "objects/group.hpp"
#include "sppmhit.hpp"

enum TraceType { EYE = 0, PHOTON };

// 记录场景中灯光和物体，进行光线追踪
class Scene {
   public:
    Scene() {}
    void trace(const Ray& r, const Vector3f& color, const Vector3f& weight, int depth, int id, TraceType type, SPPMHitPoint* hp, int incoming = 1);
    void updateHit(const Vector3f& weight, const Vector3f& point, const Vector3f& dir);
    //void buildHitKDTree();
    void buildHashGrid();
    Ray genRandomRay(int id);
    void addObject(std::shared_ptr<Object>);
    void addLight(std::shared_ptr<Light>);
    void addMesh(std::shared_ptr<Mesh>);
    void addBsplineGroup(std::shared_ptr<BsplineGroup>);

    Vector3f getFlux(int x, int numR, int numP);

    void setPicSize(int W, int H) {
        sppmhit.resize(W * H + 10);
        dataHashGrid.resize(W * H * 6 + 10);
        width = W;
        height = H;
        szSPPMHit = 0;
    }
    void initAll(double X, double Y, double Z) {
        first_init_rad = ((X + Y + Z) / 3.0) / ((width + height) / 2.0) * 2.0;
        first_init_rad *= first_init_rad;

        for (auto& a : sppmhit) a.init();
    }

    void initObjectKDTree() { objects.buildKDTree(); }

    void initRound() {
        real_sppmhits.clear();
    }

    SPPMHitPoint* getHit(int x, int y) { return &sppmhit[x * height + y]; }
    Vector3f getLightWeight() { return source->_weight(); }
    Vector3f getLightColor() { return source->_color(); }

    void updateAllFlux();

    int CNT = 0;

   private:
    ObjectGroup objects;
    std::vector<SPPMHitPoint> sppmhit;
    std::shared_ptr<Light> source;
    std::vector<RealSPPMHitPoint> real_sppmhits;
    int rtKDTree, szKDTree, width, height;
    int szSPPMHit;
    double first_init_rad;
    double init_rad;
    Vector3f minVec;

    struct HashEdge {
        int nxt;
        RealSPPMHitPoint* data;
    };
    int headHashGrid[Config::num_hash], szHashGrid[Config::num_hash];
    std::vector<HashEdge> dataHashGrid;

    //void build(int& rt, int l, int r, int d);
    //void update(int& rt, const Vector3f& weight, const Vector3f& point, const Vector3f& dir, int d = 0);
};

#endif