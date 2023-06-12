#include "DlibLandmarker.h"

namespace faces {

    DlibLandmarker::DlibLandmarker(Config const &config) {
        std::string const &model = config.getModelPath("DlibLandmarker.model");
        _load(model);
    }

    DlibLandmarker::DlibLandmarker(std::string const &model) {
        _load(model);
    }

    std::vector<cv::Point> DlibLandmarker::_detect(cv::Mat const &img) {
        dlib::cv_image<dlib::bgr_pixel> dImg = img;
        dlib::full_object_detection shape = _predictor(dImg, dlib::rectangle(img.cols, img.rows));

        std::vector<cv::Point> res;
        for (int i = 0; i < shape.num_parts(); ++i) {
            dlib::point const &dPt = shape.part(i);
            res.emplace_back(dPt.x(), dPt.y());
        }

        return res;
    }

    bool DlibLandmarker::_load(std::string const &src) {
        try {
            dlib::deserialize(src) >> _predictor;
            _ok = true;
        } catch (dlib::serialization_error &e) {
            spdlog::error("Cannot load a dlib landmark detector from {}: {}", src, e.what());
            _ok = false;
        }
        return _ok;
    }

}