#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>

#include <glm/glm.hpp>
#include <vector>

/************* Register the point 2d to boost **************/
//#include <boost/geometry/geometries/adapted/c_array.hpp>
//BOOST_GEOMETRY_REGISTER_C_ARRAY_CS(boost::geometry::cs::cartesian)
#include <boost/geometry/geometries/register/point.hpp>
BOOST_GEOMETRY_REGISTER_POINT_2D(glm::vec2, float, boost::geometry::cs::cartesian, x, y)

namespace mSynthesisPaint {
/************* Define the rect type **************/
typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<float>> mBonePolygon2D;
/******** About The calculate graph ********/
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS> mGraphType;
/******** Define the bounding box type ***********/
typedef boost::geometry::model::box<boost::geometry::model::d2::point_xy<float>> mBoundingBox2D;

struct mBone2D {
    glm::vec2 source;
    glm::vec2 target;
    float rect_width;
    mBonePolygon2D bone_polygon_2d; // from the source to the source used in boost::geometry
    int bone_index;
    bool is_inited;

    mBone2D(): is_inited(false) {}
    mBone2D(glm::vec2 source, glm::vec2 target, int bone_index, float rect_width=10);

    void initialize(glm::vec2 source, glm::vec2 target, int bone_index, float rect_width=10);

    // The function can be called only after the is_inited is true.
    bool getOverlapsWith(const mBone2D &another_bone, mBonePolygon2D &overlap_polygon);
};


int get_bone_index_from_color(glm::vec3 color);
std::vector<int> check_overlap_labels(const unsigned char * bone_map_prt, glm::u32vec3 bone_map_size, const mBonePolygon2D & overlap);
void get_overlaps_graph(const unsigned char * bone_map_ptr, glm::u32vec3 bone_map_size, const std::vector<glm::vec2>& raw_joints_2d, mGraphType & graph);
std::vector<int> get_render_order(const mGraphType & graph);
}
