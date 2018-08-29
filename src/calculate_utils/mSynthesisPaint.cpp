#include "mSynthesisPaint.h"
#include "mPoseDefs.h"
#include "mRenderParameters.h"
#include <cassert>
#include <algorithm>
namespace mSynthesisPaint {

mBone2D::mBone2D(glm::vec2 source, glm::vec2 target, int bone_index, float rect_width) {
    this->initialize(source, target, bone_index, rect_width);
}
void mBone2D::initialize(glm::vec2 source, glm::vec2 target, int bone_index, float rect_width) {
    this->bone_polygon_2d.clear();
    this->source = source;
    this->target = target;
    this->rect_width = rect_width;
    this->bone_index = bone_index;
    this->is_inited = true;

    glm::vec2 l_vec = glm::normalize(this->target - this->source);
    glm::vec2 w_vec({-l_vec.y, l_vec.x});

    // clockwise points
    glm::vec2 vertex_1 = this->source - this->rect_width / 2.0f * w_vec;
    glm::vec2 vertex_2 = this->source + this->rect_width / 2.0f * w_vec;
    glm::vec2 vertex_3 = this->target + this->rect_width / 2.0f * w_vec;
    glm::vec2 vertex_4 = this->target - this->rect_width / 2.0f * w_vec;

    std::vector<glm::vec2> bone_points({vertex_1, vertex_2, vertex_3, vertex_4, vertex_1});
    boost::geometry::append(this->bone_polygon_2d, bone_points);
}

bool mBone2D::getOverlapsWith(const mBone2D &another_bone, mBonePolygon2D &overlap_polygon) {

    assert(this->is_inited);

    std::deque<mBonePolygon2D> cur_overlaps;
    boost::geometry::intersection(this->bone_polygon_2d, another_bone.bone_polygon_2d, cur_overlaps);
    if (cur_overlaps.empty()) {
        overlap_polygon.clear();
        return false;
    }
    else {
        // cause the rectangle may only have one overlap.
        overlap_polygon = cur_overlaps.at(0);
        return true;
    }
}


int get_bone_index_from_color(glm::vec3 color) {
    int max_index = std::distance(&color[0], std::max_element(&color[0], &color[0] + 3));
//    std::cout << "max index " << max_index << "index value" << color[max_index] << std::endl;

    int val_index = static_cast<int>(std::round(color[max_index] / 0.15));

    int bone_index = 0;
    if (max_index == 0) {
        if (val_index < 4) {
            bone_index = (val_index >= 1)?val_index: 1;
        }
        else {
            bone_index = ((val_index > 6)?6:val_index) + 4;
        }
    }
    else if (max_index == 1) {
        if (std::abs(val_index - 3) < std::abs(val_index - 6)) {
            bone_index = 0;
        }
        else {
            bone_index = 7;
        }
    }
    else {
        if (val_index < 4) {
            bone_index = ((val_index >= 1)?val_index:1) + 3;
        }
        else {
            bone_index = ((val_index > 6)?6:val_index) + 7;
        }
    }
    return bone_index;
}

// TODO: The check process can be made parallel
std::vector<int> check_overlap_labels(const unsigned char * bone_map_prt, glm::u32vec3 bone_map_size, const mBonePolygon2D & overlap) {
    std::vector<int> rank(mPoseDef::num_of_bones, 0);

    mBoundingBox2D bbx;
    boost::geometry::envelope(overlap, bbx);
//    std::cout << "Overlap: " << boost::geometry::dsv(overlap) << "\n aabb: " << boost::geometry::dsv(bbx) << std::endl;

    glm::i32vec2 min_corner({std::ceil(bbx.min_corner().x()), std::ceil(bbx.min_corner().y())});
    glm::i32vec2 max_corner({std::floor(bbx.max_corner().x()), std::floor(bbx.max_corner().y())});

    for (int y = min_corner.y + 1; y < max_corner.y; ++y) {
        for (int x = min_corner.x + 1; x < max_corner.x; ++x) {
            boost::geometry::model::d2::point_xy<float> cur_point({x, y});
            if (boost::geometry::within(cur_point, overlap)) {
                // Use RGB
                glm::vec3 cur_point_color({bone_map_prt[bone_map_size.z * bone_map_size.x * y + 3 * x + 2] / 255.0f,
                                           bone_map_prt[bone_map_size.z * bone_map_size.x * y + 3 * x + 1] / 255.0f,
                                           bone_map_prt[bone_map_size.z * bone_map_size.x * y + 3 * x + 0] / 255.0f});

                int cur_label = get_bone_index_from_color(cur_point_color);
                rank[cur_label] += 1;
            }
        }
    }

    return rank;
}

/******************** I may need to handle the special case in report-8-27 ********************/
void get_overlaps_graph(const unsigned char * bone_map_ptr, glm::u32vec3 bone_map_size, const std::vector<glm::vec2>& raw_joints_2d, mGraphType & graph) {
    // Initialize the bone2d first
    std::vector<mBone2D> bones_array(mPoseDef::num_of_bones);
    for (int bone_it = 0 ; bone_it < mPoseDef::num_of_bones; ++bone_it) {
        boost::add_vertex(graph);
        glm::u32vec2 cur_bone_index = mPoseDef::bones_indices[bone_it];
        bones_array[bone_it].initialize(raw_joints_2d[cur_bone_index.x], raw_joints_2d[cur_bone_index.y], bone_it, 10);
    }

    for (int i = 0; i < mPoseDef::num_of_bones; ++i) {
        mBone2D * bone_a = &bones_array[i];
        for (int j = i + 1; j < mPoseDef::num_of_bones; ++j) {
            mBone2D * bone_b = &bones_array[j];
            mBonePolygon2D bone_overlap;
            bool is_overlapped = bone_a->getOverlapsWith(*bone_b, bone_overlap);

            // if overlapped then check the color to determine the overlap rank.
            if (is_overlapped) {
                std::vector<int> overlap_labels = check_overlap_labels(bone_map_ptr, bone_map_size, bone_overlap);
                if (overlap_labels[bone_a->bone_index] > overlap_labels[bone_b->bone_index]) {
                    boost::add_edge(bone_b->bone_index, bone_a->bone_index, graph);
                }
                else if (overlap_labels[bone_a->bone_index] < overlap_labels[bone_b->bone_index]) {
                    boost::add_edge(bone_a->bone_index, bone_b->bone_index, graph);
                }
//                else if (overlap_labels[bone_a->bone_index] != 0) {
//                    // when equal, first a then b
//                    boost::add_edge(bone_a->bone_index, bone_b->bone_index, graph);
//                }
            }
        }
    }
}
std::vector<int> get_render_order(const mGraphType & graph) {
    mGraphType cur_graph;
    boost::copy_graph(graph, cur_graph);
    std::vector<int> render_order(mPoseDef::num_of_bones, -1);
    std::vector<bool> selected_arr(mPoseDef::num_of_bones, false);

    for (int i = 0; i < mPoseDef::num_of_bones; ++i) {
        mGraphType::vertex_iterator vertexIt, vertexEnd;
        // TODO: here if there is a circle in the graph, then the bad case in report-8-27  exists!
        for (boost::tie(vertexIt, vertexEnd) = boost::vertices(cur_graph); vertexIt != vertexEnd; ++vertexIt) {
            if (!selected_arr[*vertexIt] && boost::in_degree(*vertexIt, cur_graph) == 0) {
                render_order[i] = *vertexIt;
                selected_arr[*vertexIt] = true;
                // Remove the edges of that vertexs
                boost::clear_vertex(*vertexIt, cur_graph);
                break;
            }
        }
    }
    return render_order;
}

}
