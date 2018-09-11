#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <glm/glm.hpp>
#include <vector>

#include <opencv2/core/core.hpp>

/************* Register the point 2d to boost **************/
//#include <boost/geometry/geometries/adapted/c_array.hpp>
//BOOST_GEOMETRY_REGISTER_C_ARRAY_CS(boost::geometry::cs::cartesian)
#include <boost/geometry/geometries/register/point.hpp>
BOOST_GEOMETRY_REGISTER_POINT_2D(glm::vec2, float, boost::geometry::cs::cartesian, x, y)

namespace mSynthesisPaint {
#define MY_EPSILON 0.1
/************* Define the rect type **************/
typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<float>> mBonePolygon2D;
/******** About The calculate graph ********/
typedef boost::property<boost::edge_weight_t, float> mGraphEdgeWeightProperty;
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, boost::no_property, mGraphEdgeWeightProperty> mGraphType;

/******** Define the bounding box type ***********/
typedef boost::geometry::model::box<boost::geometry::model::d2::point_xy<float>> mBoundingBox2D;

static int mSynthesisBoneWidth = 14;
static int mSynthesisJointRatio = 11;
static float mSynthesisAdjacentAreaRatio = 0.25;

struct mBone2D {
    glm::vec2 source;
    glm::vec2 target;

    glm::vec2 target_longer;

    float rect_width;
    float draw_rect_width;
    float joint_ratio;
    float draw_joint_ratio;
    mBonePolygon2D bone_polygon_2d; // from the source to the source used in boost::geometry
    int bone_index;
    bool is_inited;
    glm::vec3 bone_color;
    glm::vec3 joint_color;
    std::vector<glm::vec2> bone_points;
    glm::vec2 w_vec;
    glm::vec2 l_vec;
    float bone_length_2d;

    mBone2D(): is_inited(false) {}
    mBone2D(glm::vec2 source, glm::vec2 target, int bone_index, glm::vec3 bone_color, glm::vec3 joint_color, float rect_width=10, float joint_ratio=12);

    void initialize(glm::vec2 source, glm::vec2 target, int bone_index, glm::vec3 bone_color, glm::vec3 joint_color, float rect_width=10, float joint_ratio=12);

    // The function can be called only after the is_inited is true.
    bool getOverlapsWith(const mBone2D &another_bone, mBonePolygon2D &overlap_polygon);
    void paintOn(cv::Mat & img, glm::vec3 offset_n_scale = glm::vec3(0.f, 0.f, 1.f));
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

int get_bone_index_from_color(glm::vec3 color);
std::vector<int> check_overlap_labels(const unsigned char * bone_map_prt, glm::u32vec3 bone_map_size, const mBonePolygon2D & overlap);
std::vector<int> get_render_order(const mGraphType & graph);

/***** NOTICE: The real_joints_3d must be the joints_3d calculated by mSceneUtils::getLabelsFromFrame *****/
/***** NOTICE: The outputs of the function include the 'cropped' img for bones, 'cropped' label_2d *****/
void drawSynthesisData(const unsigned char * bone_map_ptr, glm::u32vec3 bone_map_size, std::vector<glm::vec2> & raw_joints_2d, const std::vector<glm::vec3> & real_joints_3d, cv::Mat & synthesis_img);
}
