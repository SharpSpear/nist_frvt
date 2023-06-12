#ifndef FACES_DESCRIPTOR_HPP
#define FACES_DESCRIPTOR_HPP

#include <opencv2/opencv.hpp>

#include "utils/utils.h"

namespace faces {

    /**
     * A base class for all of the face descriptors
     */
    class Descriptor {
    public:
        /**
         * Estimates descriptors for the given face image
         * It is just a wrapper around @ref _computeDescriptors, which check `_ok` and prepares an image
         *
         * @param faceImg - a face ROI
         *
         * @return a descriptor vector for the given face OR an empty vector if not `_ok`
         */
        std::vector<double> computeDescriptors(cv::Mat const &faceImg);

        /**
         * Prepares an image for descriptor by resizing it and applying a custom @ref _prepareImage method
         *
         * @param faceImg - a face image
         *
         * @return a preprocessed version of the given image
         */
        cv::Mat prepareImage(cv::Mat const &faceImg);

        /**
         * @return a value of the @ref _ok flag
         */
        [[nodiscard]] bool isOk() const {
            return _ok;
        }

    protected:
        /// the flag which indicates the readiness of the detector
        bool _ok = false;

        /// a size of the face image to pass to the detector
        FACES_DECLARE_ATTRIBUTE(cv::Size, faceSize)

        /**
         * Estimates descriptors for the given face image
         *
         * @param faceImg - a face ROI
         *
         * @return a descriptor vector for the given face
         */
        virtual std::vector<double> _computeDescriptors(cv::Mat const &faceImg) = 0;

        /**
         * A custom image preprocessing method, you may override;
         * It is called after the resizing and before descriptors computing
         *
         * @param faceImg - a face image to prepare
         */
        virtual void _prepareImage(cv::Mat &faceImg) {}

    };

}

#endif //FACES_DESCRIPTOR_HPP
