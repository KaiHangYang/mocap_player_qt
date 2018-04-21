#include "mOurOpt.h"
#include <glm/glm.hpp>
#include "mPoseDefs.h"
#include <QDebug>

namespace mOurOpt {
const double points_energy_weights[15] = {0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
const int m_root_pos = mPoseDef::root_of_joints;
const int m_bones_num = mPoseDef::num_of_bones;
const int m_joints_num = mPoseDef::num_of_joints;
const double MY_PI = 3.14159265359;
const double MY_ZERO = MY_PI / 100000;
const int discard_bones = 2;

const int bones_indices[m_joints_num][m_joints_num] = {
                                                    {-1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,-1},//0
                                                    { 0, -1, 1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1,  7},//1
                                                    {-1, 1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},//2
                                                    {-1, -1, 2, -1, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},//3
                                                    {-1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,-1},//4
                                                    {-1, 4, -1, -1, -1, -1, 5, -1, -1, -1, -1, -1, -1, -1, -1},//5
                                                    {-1, -1, -1, -1, -1, 5, -1, 6, -1, -1, -1, -1, -1, -1, -1},//6
                                                    {-1, -1, -1, -1, -1, -1, 6, -1, -1, -1, -1, -1, -1, -1,-1},//7
                                                    {-1, -1, -1, -1, -1, -1, -1, -1, -1, 9, -1, -1, -1, -1, 8},//8
                                                    {-1, -1, -1, -1, -1, -1, -1, -1, 9, -1, 10, -1, -1, -1,-1},//9
                                                    {-1, -1, -1, -1, -1, -1, -1, -1, -1, 10, -1, -1, -1,-1,-1},//10
                                                    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12,-1,11},//11
                                                    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, -1,13,-1},//12
                                                    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 13,-1,-1},//13
                                                    {-1, 7, -1, -1, -1, -1, -1, -1, 8, -1, -1, 11, -1, -1, -1},//14
                                                    };

// TODO The initial skeleton. This may depend on the real global coords
const int points_index_arr[15] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

const int bones_path_sum = 4;
const int bones_path_lens[] = {4, 4, 4, 4};
const int bones_nums[] = {
    14, 8, 9, 10,
    14, 11, 12, 13,
    1, 2, 3, 4,
    1, 5, 6, 7
};
const double bone_rel_pos[m_bones_num][3] = {
    {0.0, 1.0, 0.0}, //0
    {-1.0, 0.0, 0.0}, //1
    {-1.0, 0.0, 0.0}, //2
    {-1.0, 0.0, 0.0}, //3
    {1.0, 0.0, 0.0}, //4
    {1.0, 0.0, 0.0}, //5
    {1.0, 0.0, 0.0}, //6
    {0.0, 1.0, 0.0}, //7
    {-1.0, 0.0, 0.0}, //8
    {0.0, -1.0, 0.0}, //9
    {0.0, -1.0, 0.0}, // 10
    {1.0, 0.0, 0.0}, // 11
    {0.0, -1.0, 0.0}, // 12
    {0.0, -1.0, 0.0}, // 13
};

const int angle_params_indices[m_bones_num] = {
    -1, 0, 1, 2, 3, 4, 5, -1, 6, 7, 8, 9, 10, 11,
};
std::vector<glm::dvec3> joints_pos_keeped(m_bones_num, glm::dvec3(0.f));
std::vector<double> bones_length = mPoseDef::bones_length_dbl;
std::vector<unsigned int> bones_length_index = mPoseDef::bones_length_index;
/************************************* Parameters of the Optimizers ***************************************/
const int num_of_residuals = 3 * m_joints_num;
const int num_of_parameters = 3 + 3 + 3 * (m_bones_num - discard_bones) + 1; // test for two center opt
/***************************************************************************************************/

template<typename T>
void get_rotate_mat(T x_angle, T y_angle, T z_angle, T * r_mat) {
    T cos_gama = ceres::cos(x_angle);
    T sin_gama = ceres::sin(x_angle);

    T cos_beta = ceres::cos(y_angle);
    T sin_beta = ceres::sin(y_angle);

    T cos_alpha = ceres::cos(z_angle);
    T sin_alpha = ceres::sin(z_angle);

    T * mat_ptr = r_mat;
    mat_ptr[0] = cos_alpha * cos_beta;  mat_ptr[1] = cos_alpha * sin_beta * sin_gama - sin_alpha * cos_gama;  mat_ptr[2] = cos_alpha * sin_beta * cos_gama + sin_alpha * sin_gama;
    mat_ptr[3] = sin_alpha * cos_beta;  mat_ptr[4] = sin_alpha * sin_beta * sin_gama + cos_alpha * cos_gama;  mat_ptr[5] = sin_alpha * sin_beta * cos_gama - cos_alpha * sin_gama;
    mat_ptr[6] = -sin_beta;             mat_ptr[7] = cos_beta*sin_gama;                                       mat_ptr[8] = cos_beta * cos_gama;
}

template<typename T>
void mat4_mat4_multi(T * mat1, T * mat2, T * result) {
    // assume the mat is stored by row
    result[0] = mat1[0] * mat2[0] + mat1[1] * mat2[4] + mat1[2] * mat2[8] +  mat1[3] * mat2[12];
    result[1] = mat1[0] * mat2[1] + mat1[1] * mat2[5] + mat1[2] * mat2[9] +  mat1[3] * mat2[13];
    result[2] = mat1[0] * mat2[2] + mat1[1] * mat2[6] + mat1[2] * mat2[10] + mat1[3] * mat2[14];
    result[3] = mat1[0] * mat2[3] + mat1[1] * mat2[7] + mat1[2] * mat2[11] + mat1[3] * mat2[15];

    result[4] = mat1[4] * mat2[0] + mat1[5] * mat2[4] + mat1[6] * mat2[8] +  mat1[7] * mat2[12];
    result[5] = mat1[4] * mat2[1] + mat1[5] * mat2[5] + mat1[6] * mat2[9] +  mat1[7] * mat2[13];
    result[6] = mat1[4] * mat2[2] + mat1[5] * mat2[6] + mat1[6] * mat2[10] + mat1[7] * mat2[14];
    result[7] = mat1[4] * mat2[3] + mat1[5] * mat2[7] + mat1[6] * mat2[11] + mat1[7] * mat2[15];

    result[8] =  mat1[8] * mat2[0] + mat1[9] * mat2[4] + mat1[10] * mat2[8] +  mat1[11] * mat2[12];
    result[9] =  mat1[8] * mat2[1] + mat1[9] * mat2[5] + mat1[10] * mat2[9] +  mat1[11] * mat2[13];
    result[10] = mat1[8] * mat2[2] + mat1[9] * mat2[6] + mat1[10] * mat2[10] + mat1[11] * mat2[14];
    result[11] = mat1[8] * mat2[3] + mat1[9] * mat2[7] + mat1[10] * mat2[11] + mat1[11] * mat2[15];

    result[12] = mat1[12] * mat2[0] + mat1[13] * mat2[4] + mat1[14] * mat2[8] +  mat1[15] * mat2[12];
    result[13] = mat1[12] * mat2[1] + mat1[13] * mat2[5] + mat1[14] * mat2[9] +  mat1[15] * mat2[13];
    result[14] = mat1[12] * mat2[2] + mat1[13] * mat2[6] + mat1[14] * mat2[10] + mat1[15] * mat2[14];
    result[15] = mat1[12] * mat2[3] + mat1[13] * mat2[7] + mat1[14] * mat2[11] + mat1[15] * mat2[15];
}

template<typename T>
inline void mat3_mat3_multi(T * mat1, T * mat2, T * result) {
    result[0] = mat1[0] * mat2[0] + mat1[1] * mat2[3] + mat1[2] * mat2[6];
    result[1] = mat1[0] * mat2[1] + mat1[1] * mat2[4] + mat1[2] * mat2[7];
    result[2] = mat1[0] * mat2[2] + mat1[1] * mat2[5] + mat1[2] * mat2[8];

    result[3] = mat1[3] * mat2[0] + mat1[4] * mat2[3] + mat1[5] * mat2[6];
    result[4] = mat1[3] * mat2[1] + mat1[4] * mat2[4] + mat1[5] * mat2[7];
    result[5] = mat1[3] * mat2[2] + mat1[4] * mat2[5] + mat1[5] * mat2[8];

    result[6] = mat1[6] * mat2[0] + mat1[7] * mat2[3] + mat1[8] * mat2[6];
    result[7] = mat1[6] * mat2[1] + mat1[7] * mat2[4] + mat1[8] * mat2[7];
    result[8] = mat1[6] * mat2[2] + mat1[7] * mat2[5] + mat1[8] * mat2[8];
}

template<typename T>
inline void mat3_p_multi(T * mat1, T * p, T * result) {
    T tmp_p[3] = {p[0], p[1], p[2]};
    result[0] = mat1[0] * tmp_p[0] + mat1[1] * tmp_p[1] + mat1[2] * tmp_p[2];
    result[1] = mat1[3] * tmp_p[0] + mat1[4] * tmp_p[1] + mat1[5] * tmp_p[2];
    result[2] = mat1[6] * tmp_p[0] + mat1[7] * tmp_p[1] + mat1[8] * tmp_p[2];
}

template<typename T>
void mat4_p_multi(double const * mat1, T * p, T * result) {
    T tmp_p[4] = {p[0], p[1], p[2], p[3]};
    result[0] = T(mat1[0])  * tmp_p[0] + T(mat1[1])  * tmp_p[1] + T(mat1[2]) *  tmp_p[2] +  T(mat1[3]) * tmp_p[3];
    result[1] = T(mat1[4])  * tmp_p[0] + T(mat1[5])  * tmp_p[1] + T(mat1[6]) *  tmp_p[2] +  T(mat1[7]) * tmp_p[3];
    result[2] = T(mat1[8])  * tmp_p[0] + T(mat1[9])  * tmp_p[1] + T(mat1[10]) * tmp_p[2] +  T(mat1[11]) * tmp_p[3];
    result[3] = T(mat1[12]) * tmp_p[0] + T(mat1[13]) * tmp_p[1] + T(mat1[14]) * tmp_p[2] +  T(mat1[15]) * tmp_p[3];
}


template<typename T>
std::vector<T> points_from_angles(const T * const param) {

    T cur_rotate[9];
    T initial_rotate[9];
    T initial_rotate_2[9];
    T tmp_cur_rotate[9];
    T r_mat[9];
    T rel_cur_point[3];
    T tmp_rel_cur_point[3];

    // The first three elements is the global rotate
    // TODO I disabled some theta
    int num_of_theta = m_bones_num - discard_bones;

    const T * theta = &param[6];
    std::vector<T> result_points(3 * m_joints_num, T(0));
    /*************** Fill the known joints *****************/
    result_points[3 * 14 + 0] = T(0); result_points[3 * 14 + 1] = T(0); result_points[3 * 14 + 2] = T(0);
    result_points[3 * 0 + 0] = T(joints_pos_keeped[0].x); result_points[3 * 0 + 1] = T(joints_pos_keeped[0].y); result_points[3 * 0 + 2] = T(joints_pos_keeped[0].z);
    result_points[3 * 1 + 0] = T(joints_pos_keeped[1].x); result_points[3 * 1 + 1] = T(joints_pos_keeped[1].y); result_points[3 * 1 + 2] = T(joints_pos_keeped[1].z);


    int const * bone_num = &bones_nums[0];

    int indice;
    int tmp_index, tmp_index2;
    int bone_from, bone_to;
    T bone_len;
    T angles[3];
    
    // The root rotate angles is 0 ~ 3
    get_rotate_mat<T>(param[0], param[1], param[2], initial_rotate); // root point
    get_rotate_mat<T>(param[3], param[4], param[5], initial_rotate_2); // neck point

    for (int i = 0; i < bones_path_sum; ++i) {
        if (i < 2) {
            memcpy(cur_rotate, initial_rotate, sizeof(initial_rotate));
        }
        else {
            memcpy(cur_rotate, initial_rotate_2, sizeof(initial_rotate_2));
        }

        for (unsigned int p = 1; p < bones_path_lens[i]; ++p) {
            bone_from = *(bone_num++);
            bone_to = *(bone_num);

            indice = bones_indices[ bone_to ][ bone_from ];

            tmp_index = 3 * angle_params_indices[indice];

            if (tmp_index < 0) {
                // Only use the real direction to calculate the points,
                // Not the eular angle
                qDebug() << "Can't get here";
                exit(-1);
            }
            else {
                angles[0] = theta[tmp_index];
                angles[1] = theta[tmp_index + 1];
                angles[2] = theta[tmp_index + 2];

                get_rotate_mat(angles[0], angles[1], angles[2], r_mat);

                mat3_mat3_multi(cur_rotate, r_mat, tmp_cur_rotate);
                memcpy(cur_rotate, tmp_cur_rotate, sizeof(cur_rotate));

                bone_len = T(bones_length[bones_length_index[indice]]);

                rel_cur_point[0] = T(bone_rel_pos[indice][0] * bone_len);
                rel_cur_point[1] = T(bone_rel_pos[indice][1] * bone_len);
                rel_cur_point[2] = T(bone_rel_pos[indice][2] * bone_len);

                mat3_p_multi(cur_rotate, rel_cur_point, tmp_rel_cur_point);
                rel_cur_point[0] = tmp_rel_cur_point[0];
                rel_cur_point[1] = tmp_rel_cur_point[1];
                rel_cur_point[2] = tmp_rel_cur_point[2];

                tmp_index = 3*bone_to; tmp_index2 = 3*bone_from;
                result_points[tmp_index + 0] = result_points[tmp_index2 + 0] + rel_cur_point[0];
                result_points[tmp_index + 1] = result_points[tmp_index2 + 1] + rel_cur_point[1];
                result_points[tmp_index + 2] = result_points[tmp_index2 + 2] + rel_cur_point[2];
            }
        }
        bone_num++;
    }
    return result_points;
}

struct OurCost {
private:
    std::vector<double> points_3d;
public:
    OurCost(std::vector<double> points_3d): points_3d(points_3d) {}
    template<typename T> bool operator() (const T * const param, T * residuals) const {
        std::vector<T> new_points = points_from_angles(param);
        
        T * r_ptr = residuals;
        T * np_ptr = &new_points[0];
        T cur_scale = param[num_of_parameters - 1];

        const double * p_3d_ptr = &this->points_3d[0];

        for (int i = 0; i < m_joints_num; ++i) {
            // Depth 'IK' cost
            *(r_ptr++) = T(points_energy_weights[i]) * (*(p_3d_ptr++) - cur_scale * (*(np_ptr++)));
            *(r_ptr++) = T(points_energy_weights[i]) * (*(p_3d_ptr++) - cur_scale * (*(np_ptr++)));
            *(r_ptr++) = T(points_energy_weights[i]) * (*(p_3d_ptr++) - cur_scale * (*(np_ptr++)));
        }
        return true;
    }
    // first is the redisuals, then the size of parameters
    static ceres::CostFunction * Create(std::vector<double> points_3d) {
        return (new ceres::AutoDiffCostFunction<OurCost, num_of_residuals, num_of_parameters>(new OurCost(points_3d)));
    }
};

std::vector<double> optimize(std::vector<double> points_3d, const std::vector<double> & i_bones_length) {

    /************ Test for "keep the direction of head bone and spin bone" ***************/
    joints_pos_keeped[14] = glm::dvec3(0, 0, 0);
    joints_pos_keeped[1] = joints_pos_keeped[14] +  i_bones_length[7] * glm::normalize(glm::dvec3(points_3d[1 * 3 + 0], points_3d[1 * 3 + 1], points_3d[1 * 3 + 2]) - glm::dvec3(points_3d[14 * 3 + 0], points_3d[14 * 3 + 1], points_3d[14 * 3 + 2]));
    joints_pos_keeped[0] = joints_pos_keeped[1] + i_bones_length[0] * glm::normalize(glm::dvec3(points_3d[0 * 3 + 0], points_3d[0 * 3 + 1], points_3d[0 * 3 + 2]) - glm::dvec3(points_3d[1 * 3 + 0], points_3d[1 * 3 + 1], points_3d[1 * 3 + 2]));
    /*************************************************************************************/

    bones_length = i_bones_length;
    ceres::Problem problem;
    ceres::CostFunction * o_cost = OurCost::Create(points_3d);

    std::vector<double> params(num_of_parameters, 0);
    params[num_of_parameters - 1] = 1.0;

    problem.AddResidualBlock(o_cost, NULL, &params[0]);
    ceres::Solver::Options option;
    //option.max_num_iterations = 15;

    option.linear_solver_type = ceres::DENSE_SCHUR;
    option.trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT;
    option.minimizer_progress_to_stdout = true;
    ceres::Solver::Summary summary;

    ceres::Solve(option, &problem, &summary);

    std::vector<double> result(3*m_joints_num, 0);
    std::vector<double> tmp_point = points_from_angles<double>(&params[0]);
//    double cur_scale = params[num_of_parameters - 1];


    for (int i = 0; i < m_joints_num; ++i) {
        result[3 * i] = tmp_point[3 * i + 0];
        result[3 * i + 1] = tmp_point[3 * i + 1];
        result[3 * i + 2] = tmp_point[3 * i + 2];
    }
    return result;
}

}
