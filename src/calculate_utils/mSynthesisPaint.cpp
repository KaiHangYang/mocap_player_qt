#include "mSynthesisPaint.h"
#include "mPoseDefs.h"
#include "mRenderParameters.h"
#include <cassert>
#include <algorithm>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

namespace mSynthesisPaint {

mBone2D::mBone2D(glm::vec2 source, glm::vec2 target, int bone_index, glm::vec3 bone_color, glm::vec3 joint_color, float rect_width, float joint_ratio) {
    this->initialize(source, target, bone_index, bone_color, joint_color, rect_width);
}
void mBone2D::initialize(glm::vec2 source, glm::vec2 target, int bone_index, glm::vec3 bone_color, glm::vec3 joint_color, float rect_width, float joint_ratio) {
    this->bone_polygon_2d.clear();
    this->source = source;
    this->target = target;
    this->rect_width = rect_width;
    this->bone_index = bone_index;
    this->is_inited = true;
    this->bone_color = bone_color;
    this->joint_color = joint_color;
    this->joint_ratio = joint_ratio;

    this->bone_length_2d = glm::length(this->target - this->source);
    if (this->bone_length_2d < MY_EPSILON) {
        this->bone_length_2d = 2;
    }
    glm::vec2 l_vec = glm::normalize(this->target - this->source);
    glm::vec2 w_vec({-l_vec.y, l_vec.x});

    // clockwise points
    glm::vec2 vertex_1 = this->source - this->rect_width / 2.0f * w_vec;
    glm::vec2 vertex_2 = this->source + this->rect_width / 2.0f * w_vec;
    glm::vec2 vertex_3 = this->target + this->rect_width / 2.0f * w_vec;
    glm::vec2 vertex_4 = this->target - this->rect_width / 2.0f * w_vec;

    this->bone_points = std::vector<glm::vec2>({vertex_1, vertex_2, vertex_3, vertex_4, vertex_1});
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

void mBone2D::paintOn(cv::Mat &img) {
    assert(this->is_inited);
    // The color in cv::Mat is bgr
    cv::Scalar bone_color = cv::Scalar(static_cast<unsigned char>(255*this->bone_color.b), static_cast<unsigned char>(255 * this->bone_color.g), static_cast<unsigned char>(255 * this->bone_color.r));
    cv::Scalar joint_color = cv::Scalar(static_cast<unsigned char>(255*this->joint_color.b), static_cast<unsigned char>(255 * this->joint_color.g), static_cast<unsigned char>(255 * this->joint_color.r));

    cv::Point joint = cv::Point(this->target.x, this->target.y);

    cv::Point vertices[4];
    for (int i = 0; i < 4; ++i) {
        vertices[i] = cv::Point(this->bone_points[i].x, this->bone_points[i].y);
    }

    // First bone Then joint
    cv::fillConvexPoly(img, vertices, 4, bone_color, cv::LINE_AA);
    cv::circle(img, joint, this->joint_ratio, joint_color, CV_FILLED, cv::LINE_AA);
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
                // Use RGB, the data in cv::Mat is bgr

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

/******************** I may need to handle the special case in report-8-27 ********************/
void drawSynthesisData(const unsigned char * bone_map_ptr, glm::u32vec3 bone_map_size, const std::vector<glm::vec2> & raw_joints_2d, const std::vector<glm::vec3> & raw_joints_3d, cv::Mat & synthesis_img) {
    /**************** Calculate the relative position of the joints first ****************/
    std::vector<glm::vec3> tmpJointColors = mRenderParams::mJointColors;
    tmpJointColors[14] = glm::vec3(1.f);
    // use the hip bone as the ruler

    float relative_position_threshhold = ((glm::length(raw_joints_3d[8] - raw_joints_3d[14]) + glm::length(raw_joints_3d[11] - raw_joints_3d[14])) / 2) * 0.1;
    std::vector<bool> vertexFlags(mPoseDef::num_of_joints, false);
    for (unsigned int i = 0; i < mPoseDef::num_of_bones; ++i) {
        unsigned int line[2] = { mPoseDef::bones_indices[i].x, mPoseDef::bones_indices[i].y };

        if (!vertexFlags[line[1]]) {
            vertexFlags[line[1]] = true;

            float dert_z = std::abs(raw_joints_3d[line[0]].z) - std::abs(raw_joints_3d[line[1]].z);

            if (dert_z > relative_position_threshhold) {
                tmpJointColors[line[1]] = glm::vec3(1.f);
            }
            else if (dert_z < -relative_position_threshhold) {
                tmpJointColors[line[1]] = glm::vec3(0.f);
            }
            else {
                tmpJointColors[line[1]] = glm::vec3(0.5f);
            }
        }
    }
    /*************************************************************************************/

    /**************** Initialize the draw order graph first ******************/
    mGraphType graph;
    // Initialize the bone2d first
    std::vector<mBone2D> bones_array(mPoseDef::num_of_bones);
    for (int bone_it = 0 ; bone_it < mPoseDef::num_of_bones; ++bone_it) {
        boost::add_vertex(graph);
        glm::u32vec2 cur_bone_index = mPoseDef::bones_indices[bone_it];
        bones_array[bone_it].initialize(raw_joints_2d[cur_bone_index.x], raw_joints_2d[cur_bone_index.y], bone_it, mRenderParams::mBoneColors[bone_it], tmpJointColors[cur_bone_index.y], 15, 9);
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
    /**************** Calculate the drawing order ****************/
    std::vector<int> draw_order = get_render_order(graph);

    /**************** Then only paint the bone temporarily ***************/
    for (int bone_it = 0; bone_it < draw_order.size(); ++bone_it) {
        bones_array[draw_order[bone_it]].paintOn(synthesis_img);
    }
}


}
