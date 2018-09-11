#include <vector>
#include <glm/glm.hpp>


namespace mBBXCal {
    // x y the offset w the scale ratio
    template<typename T1, typename T2, typename T3>
    T2 crop_n_resize_joints(const std::vector<T1> & joints_2d, T3 pad_scale=0.2, int target_size=368);
}
