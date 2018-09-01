#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>

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
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS> mGraphType;
/******** Define the bounding box type ***********/
typedef boost::geometry::model::box<boost::geometry::model::d2::point_xy<float>> mBoundingBox2D;

static int mSynthesisBoneWidth = 14;
static int mSynthesisJointRatio = 11;
static float mSynthesisAdjacentAreaRatio = 0.25;

struct mBone2D {
    glm::vec2 source;
    glm::vec2 target;
    glm::vec2 raw_source;

    float rect_width;
    float joint_ratio;
    mBonePolygon2D bone_polygon_2d; // from the source to the source used in boost::geometry
    int bone_index;
    bool is_inited;
    glm::vec3 bone_color;
    glm::vec3 joint_color;
    std::vector<glm::vec2> bone_points;
    float bone_length_2d;

    mBone2D(): is_inited(false) {}
    mBone2D(glm::vec2 source, glm::vec2 target, int bone_index, glm::vec3 bone_color, glm::vec3 joint_color, float rect_width=10, float joint_ratio=12);

    void initialize(glm::vec2 source, glm::vec2 target, int bone_index, glm::vec3 bone_color, glm::vec3 joint_color, float rect_width=10, float joint_ratio=12);

    // The function can be called only after the is_inited is true.
    bool getOverlapsWith(const mBone2D &another_bone, mBonePolygon2D &overlap_polygon);
    void paintOn(cv::Mat & img);
};


int get_bone_index_from_color(glm::vec3 color);
std::vector<int> check_overlap_labels(const unsigned char * bone_map_prt, glm::u32vec3 bone_map_size, const mBonePolygon2D & overlap);
std::vector<int> get_render_order(const mGraphType & graph);

/***** NOTICE: The real_joints_3d must be the joints_3d calculated by mSceneUtils::getLabelsFromFrame *****/
void drawSynthesisData(const unsigned char * bone_map_ptr, glm::u32vec3 bone_map_size, const std::vector<glm::vec2>& raw_joints_2d, const std::vector<glm::vec3> & real_joints_3d, cv::Mat & synthesis_img);
}
