#include "Face.h"

namespace faces {

    std::vector<cv::Point> Face::getRectLandmarks() const {
        std::vector<cv::Point> res;
        std::transform(landmarks.begin(), landmarks.end(), std::back_inserter(res),
                       [&](cv::Point const &pt) -> cv::Point { return rect.tl() + pt; });
        return res;
    }
}