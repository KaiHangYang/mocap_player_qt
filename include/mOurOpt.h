#ifndef M_OUR_OPT
#define M_OUR_OPT
#include <ceres/ceres.h>
#include <cmath>
#include <iostream>
#include <cstring>
#include <ctime>
#include <cstdlib>


namespace mOurOpt {
    std::vector<double> optimize(std::vector<double> points_3d, const std::vector<double> & i_bones_length);
}
#endif
