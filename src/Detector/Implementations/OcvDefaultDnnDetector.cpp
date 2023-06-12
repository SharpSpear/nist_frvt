#include "OcvDefaultDnnDetector.h"

namespace faces {


    cv::Mat OcvDefaultDnnDetector::prepareDetectionMat(cv::Mat &detection) {
        return cv::Mat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
    }

    float OcvDefaultDnnDetector::extractConfidence(cv::Mat const &detection, int const &index) {
        return detection.at<float>(index, 2);
    }

    cv::Vec4i OcvDefaultDnnDetector::extractPoints(cv::Mat const &detection, int const &index,
                                                   cv::Size const &imgSize) {
        auto x1 = static_cast<int>(detection.at<float>(index, 3) * imgSize.width);
        auto y1 = static_cast<int>(detection.at<float>(index, 4) * imgSize.height);
        auto x2 = static_cast<int>(detection.at<float>(index, 5) * imgSize.width);
        auto y2 = static_cast<int>(detection.at<float>(index, 6) * imgSize.height);

        return {x1, y1, x2, y2};
    }
}