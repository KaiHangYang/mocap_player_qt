#include "mSynthesisPaint.h"
#include "mPoseDefs.h"
#include "mRenderParameters.h"
#include "mBBXCal.h"

#include <cassert>
#include <algorithm>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include <boost/graph/graph_utility.hpp>
#include <climits>

/******************* Bugs waiting to fixed ********************/
// 1. When the joints' endpoints are very near, the initialize program has some problems. Waiting to be fixed.
// 2. The relation of the adjacent joints may made wrong circle in the graph (Solved, I made the bone source a little short(the length of the joint ratio, but the spine is left the raw one))
/**************************************************************/

namespace mSynthesisPaint {

mBone2D::mBone2D(glm::vec2 source, glm::vec2 target, int bone_index, glm::vec3 bone_color, glm::vec3 joint_color, float rect_width, float joint_ratio) {
    this->is_inited = false;
    this->initialize(source, target, bone_index, bone_color, joint_color, rect_width, joint_ratio);
}
void mBone2D::initialize(glm::vec2 source, glm::vec2 target, int bone_index, glm::vec3 bone_color, glm::vec3 joint_color, float rect_width, float joint_ratio) {
    this->is_inited = true;
    this->bone_polygon_2d.clear();

    this->source = source;
    this->target = target;
    this->rect_width = rect_width;
    this->draw_rect_width = rect_width;
    this->joint_ratio = joint_ratio;
    this->draw_joint_ratio = this->joint_ratio;

    this->bone_index = bone_index;
    this->bone_color = bone_color;
    this->joint_color = joint_color;


    this->bone_length_2d = glm::length(this->target - this->source);
    this->l_vec = glm::normalize(this->target - this->source);
    this->target_longer = l_vec * (this->bone_length_2d + this->joint_ratio) + this->source;
    this->w_vec = glm::vec2({-l_vec.y, l_vec.x});

    // clockwise points
    glm::vec2 vertex_1 = this->source - this->rect_width / 2.0f * this->w_vec;
    glm::vec2 vertex_2 = this->source + this->rect_width / 2.0f * this->w_vec;
    glm::vec2 vertex_3 = this->target + this->rect_width / 2.0f * this->w_vec;
    glm::vec2 vertex_4 = this->target - this->rect_width / 2.0f * this->w_vec;

    std::vector<glm::vec2> bone_polygon_joints;
    if (mPoseDef::bone_is_limb[this->bone_index]) {

        bone_polygon_joints = std::vector<glm::vec2>({vertex_1, vertex_2, vertex_3, this->target_longer, vertex_4, vertex_1});
    }
    else {
        bone_polygon_joints = std::vector<glm::vec2>({vertex_1, vertex_2, vertex_3, vertex_4, vertex_1});
    }

    this->bone_points = std::vector<glm::vec2>({vertex_1, vertex_2, vertex_3, vertex_4, vertex_1});
    boost::geometry::append(this->bone_polygon_2d, bone_polygon_joints);
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

void mBone2D::paintOn(cv::Mat &img, glm::vec3 offset_n_scale) {
    assert(this->is_inited);
    // The color in cv::Mat is bgr
    cv::Scalar bone_color = cv::Scalar(static_cast<unsigned char>(255*this->bone_color.b), static_cast<unsigned char>(255 * this->bone_color.g), static_cast<unsigned char>(255 * this->bone_color.r));
    cv::Scalar joint_color = cv::Scalar(static_cast<unsigned char>(255*this->joint_color.b), static_cast<unsigned char>(255 * this->joint_color.g), static_cast<unsigned char>(255 * this->joint_color.r));


    glm::vec2 draw_source = (this->source - glm::vec2(offset_n_scale)) * offset_n_scale.z;
    glm::vec2 draw_target = (this->target - glm::vec2(offset_n_scale)) * offset_n_scale.z;
    cv::Point joint = cv::Point(std::round(draw_target.x), std::round(draw_target.y));
    // clockwise points
    glm::vec2 vertex_1 = draw_source - this->draw_rect_width / 2.0f * this->w_vec;
    glm::vec2 vertex_2 = draw_source + this->draw_rect_width / 2.0f * this->w_vec;
    glm::vec2 vertex_3 = draw_target + this->draw_rect_width / 2.0f * this->w_vec;
    glm::vec2 vertex_4 = draw_target - this->draw_rect_width / 2.0f * this->w_vec;

    cv::Point vertices[4] = {cv::Point(std::round(vertex_1.x), std::round(vertex_1.y)),
                             cv::Point(std::round(vertex_2.x), std::round(vertex_2.y)),
                             cv::Point(std::round(vertex_3.x), std::round(vertex_3.y)),
                             cv::Point(std::round(vertex_4.x), std::round(vertex_4.y))};

    // First bone Then joint
    cv::fillConvexPoly(img, vertices, 4, bone_color, cv::LINE_AA);
    cv::circle(img, joint, this->draw_joint_ratio, joint_color, CV_FILLED, cv::LINE_AA);
}

int get_bone_index_from_color(glm::vec3 color) {
    std::vector<float> color_distance(mPoseDef::num_of_bones, 0);
    glm::vec3 tmp_vec;
    for (int i = 0; i < mPoseDef::num_of_bones; ++i) {
        tmp_vec = color - mRenderParams::mBoneColors[i];
        color_distance[i] = tmp_vec.x*tmp_vec.x + tmp_vec.y*tmp_vec.y + tmp_vec.z*tmp_vec.z;
    }

    return std::distance(&color_distance[0], std::min_element(&color_distance[0], &color_distance[0] + 14));
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
void drawSynthesisData(const unsigned char * bone_map_ptr, glm::u32vec3 bone_map_size, std::vector<glm::vec2> & raw_joints_2d, const std::vector<glm::vec3> & real_joints_3d, cv::Mat & synthesis_img) {
    // The real_joints_3d is in the real world camera coordinate(the z and y axis is different with the one in the opengl)
    /**************** Calculate the relative position of the joints first ****************/
    std::vector<glm::vec3> tmpJointColors = mRenderParams::mJointColors;
    tmpJointColors[14] = glm::vec3(1.f);
    // use the hip bone as the ruler

    float relative_position_threshhold = ((glm::length(real_joints_3d[mPoseDef::left_hip_joint_index] - real_joints_3d[mPoseDef::root_of_joints]) + glm::length(real_joints_3d[mPoseDef::right_hip_joint_index] - real_joints_3d[mPoseDef::root_of_joints])) / 2) * 0.1;
    std::vector<bool> vertexFlags(mPoseDef::num_of_joints, false);
    for (unsigned int i = 0; i < mPoseDef::num_of_bones; ++i) {
        unsigned int line[2] = { mPoseDef::bones_indices[i].x, mPoseDef::bones_indices[i].y };

        if (!vertexFlags[line[1]]) {
            vertexFlags[line[1]] = true;
            // NOTICE: The z axis in the real world camera coordinate is opposite with the one in the opengl.
            float dert_z = (-real_joints_3d[line[0]].z) - (-real_joints_3d[line[1]].z);

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
    /*************************************************************************************/

    /**************** Initialize the draw order graph first ******************/
    mGraphType graph;
    // Initialize the bone2d first
    std::vector<mBone2D> bones_array(mPoseDef::num_of_bones);
    for (int bone_it = 0 ; bone_it < mPoseDef::num_of_bones; ++bone_it) {
        boost::add_vertex(graph);
        glm::u32vec2 cur_bone_index = mPoseDef::bones_indices[bone_it];
        bones_array[bone_it].initialize(raw_joints_2d[cur_bone_index.x], raw_joints_2d[cur_bone_index.y], bone_it, mRenderParams::mBoneColors[bone_it], tmpJointColors[cur_bone_index.y], mSynthesisPaint::mSynthesisBoneWidth, mSynthesisPaint::mSynthesisJointRatio);
        if (!bones_array[bone_it].is_inited) {
            std::cout << "Uninitialized bone: " << bone_it << std::endl;
        }
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
                // Use for break the cycle in the graph
                float total_valid_label = overlap_labels[bone_a->bone_index] + overlap_labels[bone_b->bone_index];
                //if (overlap_labels[bone_a->bone_index] + overlap_labels[bone_b->bone_index] < mSynthesisPaint::mSynthesisBoneWidth) {
                    // If the pixel label is very small, then discard it.
                //    continue;
                //}
                if (overlap_labels[bone_a->bone_index] > overlap_labels[bone_b->bone_index]) {
                    boost::add_edge(bone_b->bone_index, bone_a->bone_index, total_valid_label, graph);
                }
                else if (overlap_labels[bone_a->bone_index] < overlap_labels[bone_b->bone_index]) {
                    boost::add_edge(bone_a->bone_index, bone_b->bone_index, total_valid_label, graph);
                }
            }
        }
    }
    /**************** Calculate the drawing order ****************/

    // Test the cycle first
    bool has_cycle = false;
    std::vector<int> cycle_arr;
    do {
        has_cycle = false;
        cycle_arr.clear();
        dfs_cycle_detector dfs_vis(has_cycle, cycle_arr);
        boost::depth_first_search(graph, boost::visitor(dfs_vis));

        if (has_cycle) {
            int start_index = std::find(cycle_arr.begin(), cycle_arr.end(), cycle_arr.back()) - cycle_arr.begin();
            mGraphType::edge_descriptor edge_to_delete;
            float min_weight = FLT_MAX;
            for (; start_index < cycle_arr.size() - 1; ++start_index) {
                std::pair<mGraphType::edge_descriptor, bool> cur_edge = boost::edge(cycle_arr[start_index], cycle_arr[start_index + 1], graph);
                assert(cur_edge.second);
                float edge_weight = boost::get(boost::edge_weight_t(), graph, cur_edge.first);
                if (edge_weight < min_weight) {
                    min_weight = edge_weight;
                    edge_to_delete = cur_edge.first;
                }
            }
            boost::remove_edge(edge_to_delete, graph);
        }
    } while (has_cycle);

    std::vector<int> draw_order = get_render_order(graph);

    /**************** Then only paint the bone temporarily ***************/
//    std::cout << draw_order.size() << std::endl;
//    for (int i = 0; i <  draw_order.size(); ++i) {
//        std::cout << draw_order[i] << " ";
//    }
//    std::cout << std::endl;

    // Get the draw offset and scale
    glm::vec3 offset_n_scale = mBBXCal::crop_n_resize_joints(raw_joints_2d, 0.2, synthesis_img.size().width);
    for (int i = 0; i < raw_joints_2d.size(); ++i) {
        raw_joints_2d[i] = ( raw_joints_2d[i] - glm::vec2(offset_n_scale) ) * offset_n_scale.z;
    }

    for (int bone_it = 0; bone_it < draw_order.size(); ++bone_it) {
        if (draw_order[bone_it] < 0) {
            std::cout << "Something wrong happened!" << std::endl;
            boost::print_graph(graph);
            exit(-1);
            continue;
        }
        bones_array[draw_order[bone_it]].paintOn(synthesis_img, offset_n_scale);
    }
}


}
