#include "group.hpp"

void ObjectGroup::buildKDTree() {
    rtKDTree = 0;
    szKDTree = 0;
    groupKDTree.resize(group.size() * 2 + 10);
    groupKDTree[0].clr();
    build(rtKDTree, 0, group.size() - 1, 0);
}

void ObjectGroup::build(int& rt, int l, int r, int d) {
    if (!rt) {
        rt = ++szKDTree;
        groupKDTree[rt].clr();
    }
    ObjectKDTreeNode& s = groupKDTree[rt];
    if (l == r) {
        s.range = group[l]->getObjectBox();
        return;
    }
    int mid = (l + r) / 2;
    if (d == 0) {
        std::nth_element(group.begin() + l, group.begin() + mid, group.begin() + r + 1, [&](const std::shared_ptr<Object> a, const std::shared_ptr<Object> b) { return a->getObjectBox().x_range.x() < b->getObjectBox().x_range.x(); });
    } else if (d == 1) {
        std::nth_element(group.begin() + l, group.begin() + mid, group.begin() + r + 1, [&](const std::shared_ptr<Object> a, const std::shared_ptr<Object> b) { return a->getObjectBox().y_range.x() < b->getObjectBox().y_range.x(); });
    } else if (d == 2) {
        std::nth_element(group.begin() + l, group.begin() + mid, group.begin() + r + 1, [&](const std::shared_ptr<Object> a, const std::shared_ptr<Object> b) { return a->getObjectBox().z_range.x() < b->getObjectBox().z_range.x(); });
    }
    d = (d == 2 ? 0 : d + 1);
    build(s.lc, l, mid, d);
    build(s.rc, mid + 1, r, d);
    s.range = groupKDTree[s.lc].range + groupKDTree[s.rc].range;
}

std::shared_ptr<Object> ObjectGroup::intersectOnKDTree(const Ray& r, Hit& h, double tMin, int rt, int ll, int rr) {
    if (!rt) return nullptr;

    ObjectKDTreeNode& s = groupKDTree[rt];
    if (!s.check(r)) return nullptr;
    if (h.valid() && !s.check(Ray(r.pointAtParameter(h.getT()), -r.getDirection()))) return nullptr;
    int mid = (ll + rr) / 2;
    if (rr - ll <= 3) {
        return intersectNormal(r, h, tMin, ll, rr);
    }
    auto A = intersectOnKDTree(r, h, tMin, s.lc, ll, mid);
    auto B = intersectOnKDTree(r, h, tMin, s.rc, mid + 1, rr);
    if (B == nullptr) return A;
    return B;
}

std::shared_ptr<Object> ObjectGroup::intersect(const Ray& r, Hit& h, double tMin) { return intersectOnKDTree(r, h, tMin, rtKDTree, 0, group.size() - 1); }