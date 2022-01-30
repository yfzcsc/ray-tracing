#ifndef GROUP_H
#define GROUP_H

#include <iostream>
#include <vector>

#include "hit.hpp"
#include "object.hpp"
#include "ray.hpp"

class ObjectKDTreeNode {
   public:
    ObjectBox range;
    int lc, rc;
    void clr() {
        range.clr();
        lc = rc = 0;
    }
    bool check(const Ray &r) const {
        auto O = r.getOrigin(), D = r.getDirection();
        if (range.include(O)) return true;

        if (D.x() != 0) {
            auto Dx = D / D.x();
            if (range.include(O + Dx * (range.x_range.x() - O.x()))) return true;
            if (range.include(O + Dx * (range.x_range.y() - O.x()))) return true;
        }
        if (D.y() != 0) {
            auto Dx = D / D.y();
            if (range.include(O + Dx * (range.y_range.x() - O.y()))) return true;
            if (range.include(O + Dx * (range.y_range.y() - O.y()))) return true;
        }
        if (D.z() != 0) {
            auto Dx = D / D.z();
            if (range.include(O + Dx * (range.z_range.x() - O.z()))) return true;
            if (range.include(O + Dx * (range.z_range.y() - O.z()))) return true;
        }

        return false;
    }
};

class ObjectGroup {
   public:
    ObjectGroup() {}

    explicit ObjectGroup(int num_objects) { group.resize(num_objects); }

    ~ObjectGroup() {}

    std::shared_ptr<Object> intersect(const Ray &r, Hit &h, double tMin);

    void addObject(std::shared_ptr<Object> obj) { group.push_back(obj); }

    void addObject(int index, std::shared_ptr<Object> obj) { group[index] = obj; }

    int getGroupSize() { return group.size(); }

    void buildKDTree();

   private:
    std::vector<std::shared_ptr<Object>> group;
    std::vector<ObjectKDTreeNode> groupKDTree;
    int rtKDTree, szKDTree;
    void build(int &rt, int l, int r, int d);
    std::shared_ptr<Object> intersectNormal(const Ray &r, Hit &h, double tMin, int ll, int rr) {
        bool flag = false;
        std::shared_ptr<Object> ret;
        for (int i = ll; i <= rr; ++i) {
            auto &g = group[i];
            flag = g->intersect(r, h, tMin);
            if (flag) {
                ret = g;
            }
        }
        if (h.getT() < tMin) return std::shared_ptr<Object>(nullptr);
        return ret;
    }

    std::shared_ptr<Object> intersectOnKDTree(const Ray &r, Hit &h, double tMin, int rt, int ll, int rr);
};

#endif
