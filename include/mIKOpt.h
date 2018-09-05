#ifndef M_IK_OPT
#define M_IK_OPT
#include <ceres/ceres.h>
#include <cmath>
#include <iostream>
#include <cstring>
#include <ctime>
#include <cstdlib>

namespace mIKOpt {
    std::vector<double> optimizeIK(std::vector<double> points_3d, const std::vector<double> & bones_length);
    template<typename T> std::vector<T> points_from_angles(const T * const param, const std::vector<double> & bones_length);
}
#endif
