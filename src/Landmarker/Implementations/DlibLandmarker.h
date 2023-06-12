#ifndef FACES_DLIBLANDMARKER_H
#define FACES_DLIBLANDMARKER_H

#include <spdlog/spdlog.h>

#include <dlib/opencv/cv_image.h>
#include <dlib/image_processing/shape_predictor.h>

#include <utils/utils.h>
#include <Config/Config.h>
#include <Landmarker/Landmarker.hpp>

namespace faces {

    /**
     * A Dlib shape predictor-based facial landmark detector
     *
     * @see https://github.com/davisking/dlib-models#shape_predictor_5_face_landmarksdatbz2
     * @see https://github.com/davisking/dlib-models#shape_predictor_68_face_landmarksdatbz2
     */
    class DlibLandmarker : public Landmarker {
    public:
        FACES_MAIN_CONSTRUCTOR(explicit DlibLandmarker, Config const &config);

        explicit DlibLandmarker(std::string const &model);

    protected:
        dlib::shape_predictor _predictor;

        std::vector<cv::Point> _detect(cv::Mat const &img) override;

        /**
         * Deserializes a landmark detector from the given .dat file
         *
         * @param src - a file, load a detector from
         *
         * @return successfulness of the deserialization = current _ok
         */
        bool _load(std::string const &src);

    };

    FACES_REGISTER_SUBCLASS(Landmarker, DlibLandmarker, Dlib)

    FACES_AUGMENT_CONFIG(DlibLandmarker,
                         FACES_ADD_CONFIG_OPTION("DlibLandmarker.model", "mode", "", false,
                                                 "A path to a model file of Dlib-based facial landmark detector"))


}

#endif //FACES_DLIBLANDMARKER_H
