#include <iostream>
#include <glm/glm.hpp>
#include <vector>


namespace mGeo {
#define M_EPSILON 0.00001
template<typename T1, typename T2>
void checkLineSegIntersection2D(T1 p1, T1 p2, T1 p3, T1 p4, T1 & intersection, bool & is_intersect, bool & is_parallel);

template<typename T1, typename T2>
T2 getMinDistanceBetweenJointNLineSeg2D(T1 p1, T1 p2, T1 single_joint, T1 & closest_joint);

template<typename T1, typename T2>
T2 getMinDistanceBetweenLineSeg2D(T1 p1, T1 p2, T1 p3, T1 p4, T1 & closest_joint_1, T1 & closest_joint_2, bool & is_parallel);
}
