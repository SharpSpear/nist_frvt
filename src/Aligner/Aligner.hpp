#ifndef FACES_ALIGNER_HPP
#define FACES_ALIGNER_HPP

#include <Face/Face.h>
#include <Config/Config.h>

namespace faces {

    /**
     * A base class for all of the face aligners
     */
    class Aligner {
    public:
        explicit Aligner(Config const &config) {
            try {
                _faceSize = {config["Aligner.faceWidth"].getInt(),
                             config["Aligner.faceHeight"].getInt()};
            } catch (std::out_of_range &e) {
                spdlog::error("Cannot get a face size from the config!");
                _ok = false;
            }
        }

        /**
         * Aligns the given face by centering the face on its image,
         * rotating it in the way that eyes lie on a horizontal line
         * and resizing an image of the face to the appropriate size (@ref _faceSize); \n
         * This is a wrapper around the actual @ref _align method
         *
         * @param face      - a face, process the image of which
         * @param wholeImg  - an image where the face was detected
         */
        void align(Face &face, cv::Mat const &wholeImg) {
            if (!_ok) return;

            face.img = _align(face, wholeImg);
        }

        /**
         * Aligns a bunch of faces
         *
         * @overload align(Face, cv::Mat const&)
         */
        void align(std::vector<Face> &faces, cv::Mat const &wholeImg) {
            for (Face &face : faces) {
                align(face, wholeImg);
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

        /// the desired face size
        cv::Size _faceSize = {150, 150};

        /**
         * Aligns the given face by centering the face on its image,
         * rotating it in the way that eyes lie on a horizontal line
         * and resizing an image of the face to the appropriate size (@ref _faceSize)
         *
         * @param face      - a face, process the image of which
         * @param wholeImg  - an image where the face was detected
         *
         * @return an aligned version of faces image
         */
        virtual cv::Mat _align(Face const &face, cv::Mat const &wholeImg) = 0;
    };

    FACES_AUGMENT_CONFIG(Aligner,
                         FACES_ADD_CONFIG_OPTION("Aligner.faceWidth", "faceWidth", 150, false,
                                                 "A width of faces")
                                 FACES_ADD_CONFIG_OPTION("Aligner.faceHeight", "faceHeight", 150, false,
                                                         "A height of faces")
    )

}

#endif //FACES_ALIGNER_HPP
