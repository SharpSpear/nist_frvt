#ifndef FACES_FACE_H
#define FACES_FACE_H

#include <utility>

#include <opencv2/opencv.hpp>

namespace faces {

    /**
     * This is the class representing faces.
     */
    class Face {
    public:
        /// A bounding box of the face on an image
        cv::Rect rect;

        /// A predicted by the Recognizer label of this face
        int label = -1;

        /// Key points of the face (e.g. nose, eyes, mouth)
        std::vector<cv::Point> landmarks;

        /// A prepared image of the face
        cv::Mat img;

        Face() = default;

        explicit Face(cv::Rect rect)
                : rect(std::move(rect)) {}

        Face(cv::Mat img, cv::Rect rect)
                : img(std::move(img)), rect(std::move(rect)) {}

        /**
         * @return face @ref landmarks offset by the top-left corner of the @ref rect
         */
        [[nodiscard]] std::vector<cv::Point> getRectLandmarks() const;

    };

}


#endif //FACES_FACE_H
