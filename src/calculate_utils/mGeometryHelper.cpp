#include "mGeometryHelper.hpp"
#include <climits>

namespace mGeo {

template<typename T1>
T1 getTypeMaxValue();

template<>
double getTypeMaxValue<double>() {
    return DBL_MAX;
}

template<>
float getTypeMaxValue<float>() {
    return FLT_MAX;
}

template<typename T1, typename T2>
void checkLineSegIntersection2D(T1 p1, T1 p2, T1 p3, T1 p4, T1 & intersection, bool & is_intersect, bool & is_parallel) {
    is_parallel = false;
    is_intersect = false;

    T2 dx12 = p2.x - p1.x;
    T2 dy12 = p2.y - p1.y;
    T2 dx34 = p4.x - p3.x;
    T2 dy34 = p4.y - p3.y;

    T2 denominator = (dy12 * dx34 - dx12 * dy34);

    if (std::abs(denominator) < M_EPSILON) {
        // line parallel
        is_intersect = false;
        is_parallel = true;
        intersection = T1(-1, -1);
        return;
    }

    T2 t1 = ((p1.x - p3.x) * dy34 + (p3.y - p1.y) * dx34) / denominator;
    T2 t2 = ((p3.x - p1.x) * dy12 + (p1.y - p3.y) * dx12) / (-denominator);

    intersection = T1(p1.x + dx12 * t1, p1.y + dy12 * t1);
    if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
        is_intersect = true;
    }

    return;
}

template<typename T1, typename T2>
T2 getMinDistanceBetweenJointNLineSeg2D(T1 p1, T1 p2, T1 single_joint, T1 & closest_joint) {
    T2 dx12 = p2.x - p1.x;
    T2 dy12 = p2.y - p1.y;
    T2 dx13 = single_joint.x - p1.x;
    T2 dy13 = single_joint.y - p1.y;
    T2 min_distance = 0;

    T2 t = (dx12 * dx13 + dy12 * dy13) / (dx12 * dx12 + dy12 * dy12);
    if (t >= 0 && t <= 1) {
        closest_joint = T1(p1.x + t * dx12, p1.y + t * dy12);

    }
    else if (t < 0) {
        closest_joint = p1;
    }
    else if (t > 1) {
        closest_joint = p2;
    }
    min_distance = glm::length(closest_joint - single_joint);
    return min_distance;
}



template<typename T1, typename T2>
T2 getMinDistanceBetweenLineSeg2D(T1 p1, T1 p2, T1 p3, T1 p4, T1 & closest_joint_1, T1 & closest_joint_2, bool & is_parallel) {
    bool is_seg_parallel, is_seg_intersect;
    T1 intersection;
    checkLineSegIntersection2D<T1, T2>(p1, p2, p3, p4, intersection, is_seg_intersect, is_seg_parallel);
    is_parallel = is_seg_parallel;
    // If line seg intersection
    if (is_seg_intersect) {
        closest_joint_1 = intersection;
        closest_joint_2 = intersection;
        return 0;
    }

    T1 closest_tmp;
    T2 best_dist = getTypeMaxValue<T2>();
    T2 test_dist = getMinDistanceBetweenJointNLineSeg2D<T1, T2>(p3, p4, p1, closest_tmp);
    if (test_dist < best_dist) {
        best_dist = test_dist;
        closest_joint_1 = p1;
        closest_joint_2 = closest_tmp;
    }

    test_dist = getMinDistanceBetweenJointNLineSeg2D<T1, T2>(p3, p4, p2, closest_tmp);
    if (test_dist < best_dist) {
        best_dist = test_dist;
        closest_joint_1 = p2;
        closest_joint_2 = closest_tmp;
    }

    test_dist = getMinDistanceBetweenJointNLineSeg2D<T1, T2>(p1, p2, p3, closest_tmp);
    if (test_dist < best_dist) {
        best_dist = test_dist;
        closest_joint_1 = closest_tmp;
        closest_joint_2 = p3;
    }

    test_dist = getMinDistanceBetweenJointNLineSeg2D<T1, T2>(p1, p2, p4, closest_tmp);
    if (test_dist < best_dist) {
        best_dist = test_dist;
        closest_joint_1 = closest_tmp;
        closest_joint_2 = p4;
    }
    return best_dist;
}
template void checkLineSegIntersection2D<glm::f64vec2, double>(glm::f64vec2, glm::f64vec2, glm::f64vec2, glm::f64vec2, glm::f64vec2 &, bool &, bool &);
template double getMinDistanceBetweenJointNLineSeg2D<glm::f64vec2, double>(glm::f64vec2, glm::f64vec2, glm::f64vec2, glm::f64vec2 &);
template double getMinDistanceBetweenLineSeg2D<glm::f64vec2, double>(glm::f64vec2, glm::f64vec2, glm::f64vec2, glm::f64vec2, glm::f64vec2 &, glm::f64vec2 &, bool &);

}
