#include "mSynthesisPaint.h"

#include "mPoseDefs.h"
#include "mRenderParameters.h"
#include "mBBXCal.h"
#include "mGeometryHelper.hpp"

#include <cassert>
#include <algorithm>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <climits>
#include <boost/graph/graph_utility.hpp>
#include <QDebug>

namespace mSynthesisPaint {

mBone2D::mBone2D(const glm::f64vec2 &source_2d, const glm::f64vec2 &target_2d, const glm::f64vec3 &source_3d, const glm::f64vec3 &target_3d, glm::f64vec3 offset_n_scale, int bone_index, int source_index, int target_index, const glm::vec3 &bone_color, const glm::vec3 &joint_color, float bone_width, float joint_ratio) {
    this->is_inited = false;
    this->initialize(source_2d, target_2d, source_3d, target_3d, offset_n_scale, bone_index, source_index, target_index, bone_color, joint_color, bone_width, joint_ratio);
}

/****** The joints_2d is calculate from the joins_3d by the projection matrix ******/
void mBone2D::initialize(const glm::f64vec2 &source_2d, const glm::f64vec2 &target_2d, const glm::f64vec3 &source_3d, const glm::f64vec3 &target_3d, glm::f64vec3 offset_n_scale, int bone_index, int source_index, int target_index, const glm::vec3 &bone_color, const glm::vec3 &joint_color, float bone_width, float joint_ratio) {
    this->is_inited = true;
    this->source_2d_cropped = (source_2d - glm::f64vec2(offset_n_scale)) * offset_n_scale.z;
    this->target_2d_cropped = (target_2d - glm::f64vec2(offset_n_scale)) * offset_n_scale.z;
    this->source_2d = source_2d;
    this->target_2d = target_2d;
    this->source_3d = source_3d;
    this->target_3d = target_3d;

    this->bone_index = bone_index;
    this->bone_color = bone_color;
    this->joint_color = joint_color;
    this->bone_width = bone_width;
    this->joint_ratio = joint_ratio;

    this->bone_width_scaled = this->bone_width / offset_n_scale.z;
    this->joint_ratio_scaled = this->joint_ratio / offset_n_scale.z;

    this->source_index = source_index;
    this->target_index = target_index;

    this->dir_2d_cropped = glm::normalize(this->target_2d_cropped - this->source_2d_cropped);
    this->dir_2d = glm::normalize(this->target_2d - this->source_2d);
    this->dir_3d = glm::normalize(this->target_3d - this->source_3d);

    this->bone_length_3d = glm::length(this->target_3d - this->source_3d);
    this->bone_length_2d = glm::length(this->target_2d - this->source_2d);

}

int mBone2D::_checkAdjacent(const mBone2D * bone_a, const mBone2D * bone_b, int bones_joint_index) {
    glm::f64vec3 judge_point_1;
    glm::f64vec3 judge_point_2;

    double judge_len = std::min(bone_a->bone_length_3d * 0.1, bone_b->bone_length_3d * 0.1);

    if (bone_a->target_index == bones_joint_index) {
        judge_point_1 = bone_a->target_3d - judge_len * bone_a->dir_3d;
    }
    else if (bone_a->source_index == bones_joint_index) {
        judge_point_1 = bone_a->source_3d + judge_len * bone_a->dir_3d;
    }
    else {
        qDebug() << "Wrong adjancent table!";
        exit(-1);
    }

    if (bone_b->target_index == bones_joint_index) {
        judge_point_2 = bone_b->target_3d - judge_len * bone_b->dir_3d;
    }
    else if (bone_b->source_index == bones_joint_index) {
        judge_point_2 = bone_b->source_3d + judge_len * bone_b->dir_3d;
    }
    else {
        qDebug() << "Wrong adjancent table!";
        exit(-1);
    }

    if (judge_point_1.z - judge_point_2.z > M_EPSILON) {
        return 1;
    }
    else {
        return -1;
    }
}
// don't mind the 2D joint overlap, cause It will be handled by the `min distance of the segments`.
// In 3D, however, the overlap will cause no intersection of the `cross screen line` and the bone line.
// This must be handled.

bool mBone2D::_checkCropped2DOverlap(const std::vector<glm::f64vec2> & line_seg_1, const std::vector<glm::f64vec2> & line_seg_2, double threshhold, glm::f64vec2 & closest_joint_seg_1, glm::f64vec2 & closest_joint_seg_2, bool & is_parallel) {
    // check with joint_ratio_scaled.
    double min_distance;
    min_distance = mGeo::getMinDistanceBetweenLineSeg2D<glm::f64vec2, double>(line_seg_1[0], line_seg_1[1], line_seg_2[0], line_seg_2[1], closest_joint_seg_1, closest_joint_seg_2, is_parallel);

    if (min_distance >= threshhold) {
        // discard the not overlap bone. (not mind)
        return false;
    }
    else {
        return true;
    }
}

int mBone2D::checkInFrontOf(const mBone2D &another_bone, const glm::vec4 & proj_vec) {

    assert(this->is_inited);
    /*********** The nearby bones can be determined by the middle point's z ***********/
    int bones_joint_index = mPoseDef::bones_adjacence_table[this->bone_index][another_bone.bone_index];
    if (bones_joint_index > 0) {
        // The two bone is adjacent.
        return this->_checkAdjacent(this, &another_bone, bones_joint_index);
    }
    std::vector<glm::f64vec2> bone_line_seg_1({this->source_2d, this->target_2d});
    std::vector<glm::f64vec2> bone_line_seg_2({another_bone.source_2d, another_bone.target_2d});
    glm::f64vec2 closest_joint_seg_1, closest_joint_seg_2;
    bool is_segs_parallel;
    if (this->_checkCropped2DOverlap(bone_line_seg_1, bone_line_seg_2, this->joint_ratio_scaled * 2, closest_joint_seg_1, closest_joint_seg_2, is_segs_parallel)) {
        /**************** Check the bad parallel case ****************/
        if (is_segs_parallel) {
            qDebug() << "Parallel cases";
            double min_z_1 = std::min(this->source_3d.z, this->target_3d.z);
            double max_z_1 = std::max(this->source_3d.z, this->target_3d.z);

            double min_z_2 = std::min(another_bone.source_3d.z, another_bone.target_3d.z);
            double max_z_2 = std::max(another_bone.source_3d.z, another_bone.target_3d.z);

            if (min_z_1 > max_z_2) {
                return 1;
            }
            else if (min_z_2 > max_z_1) {
                return -1;
            }
            else {
                // the bad case
                qDebug() << "Bad parallel cases";
                return 0;
            }
        }
        else {
            double f_h = std::tan(proj_vec.x / 2.0f) * proj_vec.z;
            double f_w = f_h * proj_vec.y;
            // Make the cross_point [-1, 1]
            closest_joint_seg_1.x = 2 * closest_joint_seg_1.x / mRenderParams::mWindowWidth - 1;
            closest_joint_seg_1.y = 2 * (mRenderParams::mWindowHeight - 1 - closest_joint_seg_1.y) / mRenderParams::mWindowHeight - 1;
            closest_joint_seg_1.x *= f_w;
            closest_joint_seg_1.y *= f_h;

            closest_joint_seg_2.x = 2 * closest_joint_seg_2.x / mRenderParams::mWindowWidth - 1;
            closest_joint_seg_2.y = 2 * (mRenderParams::mWindowHeight - 1 - closest_joint_seg_2.y) / mRenderParams::mWindowHeight - 1;
            closest_joint_seg_2.x *= f_w;
            closest_joint_seg_2.y *= f_h;

            glm::f64vec3 cross_line_vec = glm::normalize(glm::f64vec3(0, 0, -proj_vec.z) + glm::f64vec3(closest_joint_seg_1.x, closest_joint_seg_1.y, 0));
            glm::f64vec3 a1 = this->dir_3d;
            glm::f64vec3 b1 = cross_line_vec;
            glm::f64vec3 c1 = glm::f64vec3(0) - this->source_3d;

            // handle the 2D joint overlap case
            glm::f64vec3 cross_point_3d_1;
            double denominator_1 = std::pow(glm::length(glm::cross(a1, b1)), 2);

            if (denominator_1 <= M_PARALLEL_EPSILON) {
                // the two may be parallel
                // then the cross point is the one near the screen
                if (this->source_3d.z > this->target_3d.z) {
                    cross_point_3d_1 = this->source_3d;
                }
                else {
                    cross_point_3d_1 = this->target_3d;
                }
            }
            else {
                double l1_3d = glm::dot(glm::cross(c1, b1), glm::cross(a1, b1)) / denominator_1;
                cross_point_3d_1 = this->source_3d + this->dir_3d * l1_3d;
            }

            cross_line_vec = glm::normalize(glm::f64vec3(0, 0, -proj_vec.z) + glm::f64vec3(closest_joint_seg_2.x, closest_joint_seg_2.y, 0));
            glm::f64vec3 a2 = another_bone.dir_3d;
            glm::f64vec3 b2 = cross_line_vec;
            glm::f64vec3 c2 = glm::f64vec3(0) - another_bone.source_3d;

            double denominator_2 = std::pow(glm::length(glm::cross(a2, b2)), 2);
            glm::f64vec3 cross_point_3d_2;

            if (denominator_2 <= M_PARALLEL_EPSILON) {
                if (another_bone.source_3d.z > another_bone.target_3d.z) {
                    cross_point_3d_2 = another_bone.source_3d;
                }
                else {
                    cross_point_3d_2 = another_bone.target_3d;
                }

            }
            else {
                double l2_3d = glm::dot(glm::cross(c2, b2), glm::cross(a2, b2)) / denominator_2;
                cross_point_3d_2 = another_bone.source_3d + another_bone.dir_3d * l2_3d;
            }

            if (cross_point_3d_1.z > cross_point_3d_2.z) {
                return 1;
            }
            else {
                return -1;
            }
        }

    }
    return 2;
}
void mBone2D::paintOn(cv::Mat &img) {
    assert(this->is_inited);
    // The color in cv::Mat is bgr
    cv::Scalar bone_color = cv::Scalar(static_cast<unsigned char>(255*this->bone_color.b), static_cast<unsigned char>(255 * this->bone_color.g), static_cast<unsigned char>(255 * this->bone_color.r));
    cv::Scalar joint_color = cv::Scalar(static_cast<unsigned char>(255*this->joint_color.b), static_cast<unsigned char>(255 * this->joint_color.g), static_cast<unsigned char>(255 * this->joint_color.r));

    glm::vec2 draw_source(this->source_2d_cropped);
    glm::vec2 draw_target(this->target_2d_cropped);

    cv::Point joint = cv::Point(std::round(draw_target.x), std::round(draw_target.y));
    glm::vec2 w_vec(this->dir_2d.y, -this->dir_2d.x);
    // clockwise points
    glm::vec2 vertex_1 = draw_source - this->bone_width / 2.0f * w_vec;
    glm::vec2 vertex_2 = draw_source + this->bone_width / 2.0f * w_vec;
    glm::vec2 vertex_3 = draw_target + this->bone_width / 2.0f * w_vec;
    glm::vec2 vertex_4 = draw_target - this->bone_width / 2.0f * w_vec;

    cv::Point vertices[4] = {cv::Point(std::round(vertex_1.x), std::round(vertex_1.y)),
                             cv::Point(std::round(vertex_2.x), std::round(vertex_2.y)),
                             cv::Point(std::round(vertex_3.x), std::round(vertex_3.y)),
                             cv::Point(std::round(vertex_4.x), std::round(vertex_4.y))};

    // First bone Then joint
    cv::fillConvexPoly(img, vertices, 4, bone_color, cv::LINE_AA);
    cv::circle(img, joint, this->joint_ratio, joint_color, CV_FILLED, cv::LINE_AA);
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
    // For debug
//    for (int i = 0; i < render_order.size(); ++i) {
//        if (render_order[i] == -1) {
//            boost::print_graph(cur_graph);
//            break;
//        }
//    }
    return render_order;
}
bool drawCroppedSynthesisData(const std::vector<glm::f64vec2> & raw_joints_2d, const std::vector<glm::f64vec3> & raw_joints_3d, const glm::vec4 & proj_vec, std::vector<glm::vec2> & labels_2d_cropped, cv::Mat & synthesis_img) {
    bool is_syn_ok = true;
    std::vector<glm::vec3> tmpJointColors = mRenderParams::mJointColors;
    /********************* TODO Calculate the joints position relation ***********************/
    tmpJointColors[14] = glm::vec3(1.f);
    std::vector<bool> vertexFlags(mPoseDef::num_of_joints, false);

    double relative_position_threshhold = ((glm::length(raw_joints_3d[mPoseDef::left_hip_joint_index] - raw_joints_3d[mPoseDef::root_of_joints]) + glm::length(raw_joints_3d[mPoseDef::right_hip_joint_index] - raw_joints_3d[mPoseDef::root_of_joints])) / 2) * 0.1;
    for (unsigned int i = 0; i < mPoseDef::num_of_bones; ++i) {
        unsigned int line[2] = { mPoseDef::bones_indices[i].x, mPoseDef::bones_indices[i].y };
        if (!vertexFlags[line[1]]) {
            vertexFlags[line[1]] = true;
            float dert_z = raw_joints_3d[line[0]].z - raw_joints_3d[line[1]].z;

            if (dert_z < -relative_position_threshhold) {
                tmpJointColors[line[1]] = glm::vec3(1.f);
            }
            else if (dert_z > relative_position_threshhold) {
                tmpJointColors[line[1]] = glm::vec3(0.f);
            }
            else {
                tmpJointColors[line[1]] = glm::vec3(0.5f);
            }
        }
    }
    /*****************************************************************************************/

    /**************** First calculate the scaled joints_2d and joints_2d_f64 ****************/
    glm::f64vec3 offset_n_scale = mBBXCal::crop_n_resize_joints<glm::f64vec2, glm::f64vec3, double>(raw_joints_2d, 0.2, synthesis_img.size().width);
    // crop the labels for save
    for (int i = 0; i < labels_2d_cropped.size(); ++i) {
        labels_2d_cropped[i] = ( labels_2d_cropped[i] - glm::vec2(offset_n_scale) ) * static_cast<float>(offset_n_scale.z);
    }
    /**************** Then initialize the draw order graph ******************/
    mGraphType graph;
    // Initialize the bone2d first
    std::vector<mBone2D> bones_array(mPoseDef::num_of_bones);
    for (int bone_it = 0 ; bone_it < mPoseDef::num_of_bones; ++bone_it) {
        boost::add_vertex(graph);
        glm::u32vec2 cur_bone_index = mPoseDef::bones_indices[bone_it];
        bones_array[bone_it].initialize(raw_joints_2d[cur_bone_index.x], raw_joints_2d[cur_bone_index.y], raw_joints_3d[cur_bone_index.x], raw_joints_3d[cur_bone_index.y], offset_n_scale, bone_it, cur_bone_index.x, cur_bone_index.y, mRenderParams::mBoneColors[bone_it], tmpJointColors[cur_bone_index.y], mSynthesisPaint::mSynthesisBoneWidth, mSynthesisPaint::mSynthesisJointRatio);
        if (!bones_array[bone_it].is_inited) {
            std::cout << "Uninitialized bone: " << bone_it << std::endl;
        }
    }

    for (int i = 0; i < mPoseDef::num_of_bones; ++i) {
        mBone2D * bone_a = &bones_array[i];
        for (int j = i + 1; j < mPoseDef::num_of_bones; ++j) {
            mBone2D * bone_b = &bones_array[j];
            int position_flag = bone_a->checkInFrontOf(*bone_b, proj_vec);
            if (position_flag == 1) {
                boost::add_edge(bone_b->bone_index, bone_a->bone_index, 0, graph);
            }
            else if (position_flag == -1) {
                boost::add_edge(bone_a->bone_index, bone_b->bone_index, 0, graph);
            }
            else if (position_flag == 0) {
                // the bad parallel case
                is_syn_ok = false;
                return is_syn_ok;
            }
        }
    }

    std::vector<int> draw_order = get_render_order(graph);
    for (int bone_it = 0; bone_it < draw_order.size(); ++bone_it) {
        // check the cycle
        if (draw_order[bone_it] < 0) {
            is_syn_ok = false;
            return is_syn_ok; // Here the synthesis is failed deal to the cycle in the graph
        }
    }

    for (int bone_it = 0; bone_it < draw_order.size(); ++bone_it) {
        bones_array[draw_order[bone_it]].paintOn(synthesis_img);
    }

    return is_syn_ok;
}

}
