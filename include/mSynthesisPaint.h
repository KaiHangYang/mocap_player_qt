#include <glm/glm.hpp>
#include <vector>
#include <opencv2/core/core.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <opencv2/core/core.hpp>


namespace mSynthesisPaint {

#define M_EPSILON 0.0000001
#define M_PARALLEL_EPSILON 0.001
//static int mSynthesisBoneWidth = 14;
//static int mSynthesisJointRatio = 11;
static int mSynthesisBoneWidth = 10;
static int mSynthesisJointRatio = 7;
/******** About The calculate graph ********/
typedef boost::property<boost::edge_weight_t, float> mGraphEdgeWeightProperty;
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, boost::no_property, mGraphEdgeWeightProperty> mGraphType;

inline double m_cross_2d(const glm::f64vec2 & a, const glm::f64vec2 & b) {
    return a.x * b.y - b.x * a.y;
}

struct mBone2D {

    glm::f64vec2 source_2d_cropped;
    glm::f64vec2 target_2d_cropped;

    glm::f64vec2 source_2d;
    glm::f64vec2 target_2d;
    glm::f64vec3 source_3d;
    glm::f64vec3 target_3d;

    float bone_width;
    float joint_ratio;

    float bone_width_scaled;
    float joint_ratio_scaled;

    int bone_index;
    glm::vec3 bone_color;
    glm::vec3 joint_color;

    glm::f64vec2 dir_2d_cropped;
    glm::f64vec3 dir_3d;
    glm::f64vec2 dir_2d;

    int source_index;
    int target_index;

    bool is_inited;

    double bone_length_3d;
    double bone_length_2d;

    mBone2D(): is_inited(false) {}
    mBone2D(const glm::f64vec2 & source_2d, const glm::f64vec2 & target_2d, const glm::f64vec3 & source_3d, const glm::f64vec3 & target_3d, glm::f64vec3 offset_n_scale, int bone_index, int source_index, int target_index, const glm::vec3 & bone_color, const glm::vec3 & joint_color, float bone_width=mSynthesisBoneWidth, float joint_ratio=mSynthesisJointRatio);
    void initialize(const glm::f64vec2 & source_2d, const glm::f64vec2 & target_2d, const glm::f64vec3 & source_3d, const glm::f64vec3 & target_3d, glm::f64vec3 offset_n_scale, int bone_index, int source_index, int target_index, const glm::vec3 & bone_color, const glm::vec3 & joint_color, float bone_width=mSynthesisBoneWidth, float joint_ratio=mSynthesisJointRatio);
    // is_discard is true, if the bone_width is not fit for the pose. (The bone_rect is too wide)
    // 2 is not mind,  1 is ahead, 0 is not valid, -1 is behind,
    int checkInFrontOf(const mBone2D & another_bone, const glm::vec4 & proj_vec);
    void paintOn(cv::Mat & img);

    static int _checkAdjacent(const mBone2D * bone_a, const mBone2D * bone_b, int bones_joint_index);
    static bool _checkCropped2DOverlap(const std::vector<glm::f64vec2> & line_seg_1, const std::vector<glm::f64vec2> & line_set_2, double threshhold, glm::f64vec2 & closest_joint_seg_1, glm::f64vec2 & closest_joint_seg_2, bool & is_parallel);
};


/************ depth_first_search visitor to detector a cycle from the graph *************/
struct dfs_cycle_detector: public boost::dfs_visitor<> {
    dfs_cycle_detector(bool & has_cycle, std::vector<int> & cycle_arr): _has_cycle(has_cycle), _cycle_arr(cycle_arr) {}
    template <class Vertex, class Graph>
    void discover_vertex(Vertex v, const Graph & g) {
        this->_vis_arr.push_back(v);
    }

    template <class Vertex, class Graph>
    void finish_vertex(Vertex v, const Graph & g) {
        this->_vis_arr.erase(this->_vis_arr.end() - 1);
    }

    template <class Edge, class Graph>
    void back_edge(Edge e, const Graph & g) {
        this->_has_cycle = true;
        this->_cycle_arr = this->_vis_arr;
        this->_cycle_arr.push_back(boost::target(e, g));
    }
protected:
    bool & _has_cycle;
    std::vector<int> _vis_arr;
    std::vector<int> & _cycle_arr;

};

std::vector<int> get_render_order(const mGraphType & graph);
bool drawCroppedSynthesisData(const std::vector<glm::f64vec2> & raw_joints_2d, const std::vector<glm::f64vec3> & raw_joints_3d, const glm::vec4 & proj_vec, std::vector<glm::vec2> & labels_2d_cropped, cv::Mat & synthesis_img);

}
