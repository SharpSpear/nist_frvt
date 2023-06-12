#ifndef FACES_OCVDEFAULTDNNDETECTOR_H
#define FACES_OCVDEFAULTDNNDETECTOR_H

#include "Detector/OcvDnnDetector.h"


namespace faces {

    /**
     * A class for default opencv DNN
     *
     * @see https://github.com/opencv/opencv/blob/3.4.3/samples/dnn/face_detector/how_to_train_face_detector.txt
     */
    class OcvDefaultDnnDetector : public OcvDnnDetector {
    public:
        FACES_OVERRIDE_ATTRIBUTE(confidenceThreshold, 0.7)

        using OcvDnnDetector::OcvDnnDetector;

    protected:
        FACES_OVERRIDE_ATTRIBUTE(inSize, 300, 300);
        FACES_OVERRIDE_ATTRIBUTE(inScaleFactor, 1.0)
        FACES_OVERRIDE_ATTRIBUTE(meanVal, 104.0, 177.0, 123.0)
        FACES_OVERRIDE_ATTRIBUTE(swaptRB, false)

        FACES_OVERRIDE_ATTRIBUTE(inputName, "data")
        FACES_OVERRIDE_ATTRIBUTE(outputName, "detection_out")

        cv::Mat prepareDetectionMat(cv::Mat &detection) override;

        float extractConfidence(cv::Mat const &detection, int const &index) override;

        cv::Vec4i extractPoints(cv::Mat const &detection, int const &index, cv::Size const &imgSize) override;
    };

    FACES_REGISTER_SUBCLASS(Detector, OcvDefaultDnnDetector, OcvDefaultDnn)

}

#endif //FACES_OCVDEFAULTDNNDETECTOR_H
