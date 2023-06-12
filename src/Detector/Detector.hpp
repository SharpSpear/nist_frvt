#ifndef FACES_DETECTOR_HPP
#define FACES_DETECTOR_HPP

#include <vector>

#include <opencv2/opencv.hpp>

#include "Face/Face.h"
#include "utils/utils.h"

namespace faces {

    /**
     * An abstract base class for all of the face detectors
     */
    class Detector {
    public:
        /**
         * Detect faces on the given image
         * This is a wrapper around the actual detection method, which is just checking the @ref _ok flag
         *
         * @param img - image, detect faces on
         *
         * @return a vector of detected faces OR an empty vector, in case @ref _ok was set to `false`
         */
        std::vector<Face> detect(cv::Mat const &img) {
            if (!_ok) {
                return {};
            }
            return _detect(img);
        }

        /**
         * @return a value of the @ref _ok flag
         */
        [[nodiscard]] bool isOk() const {
            return _ok;
        }

    protected:
        /// the flag which indicates the readiness of the detector
        bool _ok = false;

        /**
         * The method which actually performs face detection
         *
         * @param img - image, detect faces on
         *
         * @return a vector of detected faces
         */
        virtual std::vector<Face> _detect(cv::Mat const &img) = 0;

    };

}

#endif //FACES_DETECTOR_HPP
