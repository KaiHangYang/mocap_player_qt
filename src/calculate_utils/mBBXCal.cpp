#include "mBBXCal.h"
#include <climits>

namespace mBBXCal {
glm::vec3 crop_n_resize_joints(const std::vector<glm::vec2> & joints_2d, float pad_scale, int target_size) {
    /************** Get the tight bounding box first ****************/
    float min_x = FLT_MAX;
    float min_y = FLT_MAX;
    float max_x = FLT_MIN;
    float max_y = FLT_MIN;
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
    glm::vec2 bbx_center((min_x + max_x) / 2.0f, (min_y + max_y) / 2.f);
    float bbx_size = std::max((max_x - min_x), (max_y - min_y)) * (1.f + pad_scale);
    glm::vec3 result;

    result.x = bbx_center.x - bbx_size / 2.f;
    result.y = bbx_center.y - bbx_size / 2.f;
    result.z = target_size / bbx_size;

    return result;
}
}

