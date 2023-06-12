#ifndef FACES_OCVDNNDETECTOR_H
#define FACES_OCVDNNDETECTOR_H

#include <spdlog/spdlog.h>

#include <opencv2/dnn.hpp>
#include <utility>

#include <Config/Config.h>

#include "Detector.hpp"

namespace faces {

    /**
     * A base class for opencv DNN based face detectors
     */
    class OcvDnnDetector : public Detector {
    public:
        FACES_DECLARE_ATTRIBUTE(float, confidenceThreshold)

        FACES_MAIN_CONSTRUCTOR(explicit OcvDnnDetector, Config const &config);

        /**
         * Initializes the object and loads a neural network from the given files
         *
         * @see readNet
         */
        OcvDnnDetector(std::string const &configFile, std::string const &weightFile);

        /**
         * Reads a DNN from the given files
         *
         * @param configFile - configuration file
         * @param weightFile - weights file
         *
         * @see cv::dnn::readNet
         *
         * @return was network loaded successfully or not
         */
        bool readNet(std::string const &configFile, std::string const &weightFile);

    protected:
        cv::dnn::Net net;

        FACES_DECLARE_ATTRIBUTE(cv::Size, inSize)

        FACES_DECLARE_ATTRIBUTE(double, inScaleFactor)

        FACES_DECLARE_ATTRIBUTE(cv::Scalar, meanVal)

        FACES_DECLARE_ATTRIBUTE(bool, swaptRB)

        FACES_DECLARE_ATTRIBUTE(cv::String, inputName)

        FACES_DECLARE_ATTRIBUTE(cv::String, outputName)

        /**
         * Forwards a given image through the neural network and does some other stuff
         */
        std::vector<Face> _detect(cv::Mat const &img) override;

        /**
         * Creates a blob from the given image, to fit into the DNN
         *
         * @param img - image, create blob from
         *
         * @return a blob, valid to fit into the DNN
         */
        cv::Mat createBlob(cv::Mat const &img);

        /**
         * Forwards the given blob through the DNN
         *
         * @param blob - a blob obtained from the @ref createBlob
         *
         * @return prediction of the neural network
         */
        cv::Mat forwardNet(cv::Mat const &blob);

        /**
         * Creates a matrix for us to iterate through from the DNN result
         *
         * @param detection - a matrix obtained from the DNN in @ref forwardNet
         *
         * @return a prepared matrix
         */
        virtual cv::Mat prepareDetectionMat(cv::Mat &detection) = 0;

        /**
         * Extracts a maximum value for iterating through the matrix. \n
         * It may be `detection.rows` or `detection.cols`
         *
         * @param detection - a matrix obtained from @ref prepareDetectionMat
         *
         * @return a maximum index iterate to
         */
        static int extractIterationLimit(cv::Mat const &detection) { return detection.rows; }

        /**
         * Extracts the confidence of the prediction
         *
         * @param detection - a matrix obtained from @ref prepareDetectionMat
         * @param index     - index of the prediction, get confidence for
         *
         * @return a confidence of the prediction at the given index
         */
        virtual float extractConfidence(cv::Mat const &detection, int const &index) = 0;

        /**
         * Extracts points of the prediction
         *
         * @param detection - a matrix obtained from @ref prepareDetectionMat
         * @param index     - index of the prediction
         * @param imgSize   - size of the image
         *
         * @return predicted points in format `{top_left_x, top_left_y, bottom_right_x, bottom_right_y}`
         */
        virtual cv::Vec4i extractPoints(cv::Mat const &detection,
                                        int const &index,
                                        cv::Size const &imgSize) = 0;

    };

    FACES_AUGMENT_CONFIG(OcvDnnDetector,
                         FACES_ADD_CONFIG_OPTION("OcvDnnDetector.configFile", "configFile", "",
                                                 false,
                                                 "A path to a config file of OpenCV DNN-based face detector")
                                 FACES_ADD_CONFIG_OPTION("OcvDnnDetector.weightFile", "weightFile", "",
                                                         false,
                                                         "A path to a config file of OpenCV DNN-based face detector")
    )

}


#endif //FACES_OCVDNNDETECTOR_H
