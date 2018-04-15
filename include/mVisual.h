#ifndef M_VISUAL
#define M_VISUAL

#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <glm/glm.hpp>

namespace mVTools {
    std::vector<unsigned char> getColor(float value, float max_val, float min_val, float mid_val);
    void makeGaussian(cv::Mat & heatmap, float center_x, float center_y, float r);
    void visualHeatmap(const cv::Mat & heatmap, cv::Mat &result, float max_val, float min_val, float mid_val = 0);
    void visualPafmap(const cv::Mat & paf_x, const cv::Mat & paf_y, cv::Mat & result, float max_val, float min_val, float mid_val);
    void visualDzfmap(const cv::Mat & dzf, cv::Mat & result, float max_val, float min_val, float mid_val);
    void getRangeMap(cv::Mat & range_map, float max_val, float min_val, float mid_val = 0, float type = 0);
    void pixelWiseMinMax(const cv::Mat & mat1, cv::Mat & result_mat, int type=0);

    void drawLines(cv::Mat img, std::vector<glm::vec2> points);
    void drawPoints(cv::Mat img, std::vector<glm::vec2> points);
}
#endif
