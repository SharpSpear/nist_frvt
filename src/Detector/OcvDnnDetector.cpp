#include "OcvDnnDetector.h"

namespace faces {

    OcvDnnDetector::OcvDnnDetector(Config const &config) {
        std::string configFile = config.getModelPath("OcvDnnDetector.configFile");
        std::string weightFile = config.getModelPath("OcvDnnDetector.weightFile");
        if (readNet(configFile, weightFile)) {
            _ok = true;
        }
    }

    OcvDnnDetector::OcvDnnDetector(std::string const &configFile, std::string const &weightFile) {
        if (readNet(configFile, weightFile)) {
            _ok = true;
        }
    }

    bool OcvDnnDetector::readNet(std::string const &configFile, std::string const &weightFile) {
        std::string error;
        try {
            net = cv::dnn::readNet(configFile, weightFile);
        } catch (const cv::Exception &e) {
            error = e.err;
        }
        if (net.empty()) {
            spdlog::error("Could not load net with config file '{}', and weights '{}'{}",
                          configFile, weightFile, (error.empty() ? "" : "\n\t\t: " + error));
            return false;
        }
        return true;
    }

    std::vector<Face> OcvDnnDetector::_detect(cv::Mat const &img) {
        std::vector<Face> res;

        cv::Mat detection = forwardNet(img);
        cv::Mat detectionMat = prepareDetectionMat(detection);

        for (int i = 0; i < extractIterationLimit(detectionMat); ++i) {
            float confidence = extractConfidence(detectionMat, i);
            if (confidence < get_confidenceThreshold()) {
                continue;
            }

            cv::Vec4i cords = extractPoints(detectionMat, i, img.size());

            cv::Rect faceRect(cv::Point(cords[0], cords[1]),
                              cv::Point(cords[2], cords[3]));
            // constrain the rect within the image boundaries
            faceRect &= cv::Rect({0, 0}, img.size());
            cv::Mat faceRoi(img(faceRect));
            Face f(faceRoi, faceRect);
            res.emplace_back(f);
        }

        return res;
    }

    cv::Mat OcvDnnDetector::createBlob(cv::Mat const &img) {
        return cv::dnn::blobFromImage(img, get_inScaleFactor(), get_inSize(), get_meanVal(),
                                      get_swaptRB(), false);
    }

    cv::Mat OcvDnnDetector::forwardNet(const cv::Mat &blob) {
        cv::Mat inputBlob = createBlob(blob);
        net.setInput(inputBlob, get_inputName());
        return net.forward(get_outputName());
    }

}