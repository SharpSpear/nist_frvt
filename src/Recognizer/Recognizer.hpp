#ifndef FACES_RECOGNIZER_HPP
#define FACES_RECOGNIZER_HPP

#include "Face/Face.h"

namespace faces {

    /**
     * A base class for all of the face recognizers
     */
    class Recognizer {
    public:
        /**
         * Estimate a label of the given face image.
         * This is a wrapper around the actual recognition method, which is just checking the @ref _ok flag
         *
         * @param face - the face, estimate a label for img of which
         */
        void recognize(Face &face) {
            if (!_ok) {
                return;
            }

            if (face.img.empty()) return;
            face.label = _recognize(face.img);
        }

        /**
         * A helper method which recognizes a bunch of faces
         *
         * @param faces - faces, estimate a label for img of which
         */
        void recognize(std::vector<Face> &faces) {
            for (auto &face : faces) {
                recognize(face);
            }
        }

        /**
         * Train a detector
         *
         * @param samples - a map in a format {label: face ROI}
         */
        virtual void train(std::map<int, cv::Mat &> const &samples) = 0;

        /**
         * @return a value of the @ref _ok flag
         */
        [[nodiscard]] bool isOk() const {
            return _ok;
        }

    protected:
        /// the flag which indicates the readiness of the recognizer
        bool _ok = false;

        /**
         * Estimate a label of the given face image
         *
         * @param img - face ROI
         *
         * @returns a label of the face
         */
        virtual int _recognize(cv::Mat const &img) = 0;

    };

}

#endif //FACES_RECOGNIZER_HPP
