#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "vecmath.h"

namespace Config {

const double inf = 1e30;
const double gamma = 0.5;
const double alpha = 0.7;
const int depth_limit = 15;
const double tMin = 1e-3;
const double epsilon = 1e-7;
const double pi = acos(-1.0);
const int num_hash = 1e6 + 3;
const int Bdegree = 3;

}  // namespace Config

#endif