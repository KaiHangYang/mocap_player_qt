#include "mBBXCal.h"
#include <climits>

namespace mBBXCal {
template<typename T1, typename T2, typename T3>
T2 crop_n_resize_joints(const std::vector<T1> & joints_2d, T3 pad_scale, int target_size) {
    /************** Get the tight bounding box first ****************/
    T3 min_x = FLT_MAX;
    T3 min_y = FLT_MAX;
    T3 max_x = FLT_MIN;
    T3 max_y = FLT_MIN;
    for (int i = 0; i < joints_2d.size(); ++i) {
        if (joints_2d[i].x < min_x) {
            min_x = joints_2d[i].x;
        }
        if (joints_2d[i].x > max_x) {
            max_x = joints_2d[i].x;
        }
        if (joints_2d[i].y < min_y) {
            min_y = joints_2d[i].y;
        }
        if (joints_2d[i].y > max_y) {
            max_y = joints_2d[i].y;
        }
    }

    /************* Then set pad to the bbx *************/
    T1 bbx_center((min_x + max_x) / static_cast<T3>(2.0), (min_y + max_y) / static_cast<T3>(2));
    T3 bbx_size = std::max((max_x - min_x), (max_y - min_y)) * (static_cast<T3>(1.) + pad_scale);
    T2 result;

    result.x = bbx_center.x - bbx_size / 2.f;
    result.y = bbx_center.y - bbx_size / 2.f;
    result.z = target_size / bbx_size;

    return result;
}

template glm::f64vec3 crop_n_resize_joints<glm::f64vec2, glm::f64vec3, double>(const std::vector<glm::f64vec2> & joints_2d, double pad_scale, int target_size);

}

