#include <vector>
#include <glm/glm.hpp>


namespace mBBXCal {
    // x y the offset w the scale ratio
    glm::vec3 crop_n_resize_joints(const std::vector<glm::vec2> & joints_2d, float pad_scale=0.2, int target_size=368);
}
