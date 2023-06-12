#ifndef FACES_LANDMARKER_HPP
#define FACES_LANDMARKER_HPP


#include <Face/Face.h>

namespace faces {

    /**
     * A base class for all of the face landmark detectors
     */
    class Landmarker {
    public:
        /**
         * Detects landmarks of the given face
         *
         * @param face - a face, detect landmarks for the image of which
         */
        void detect(Face &face) {
            if (!_ok) return;

            face.landmarks = _detect(face.img);
        }

        /**
         * Detects landmarks of the given faces
         *
         * @param faces - faces, detect landmarks for the image of which
         */
        void detect(std::vector<Face> &faces) {
            for (Face &face : faces) {
                detect(face);
            }
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
         * Detects landmarks for the face on the given image
         *
         * @param img - a photo of the face
         *
         * @return a vector of face landmarks
         */
        virtual std::vector<cv::Point> _detect(cv::Mat const &img) = 0;

    };

}

#endif //FACES_LANDMARKER_HPP
