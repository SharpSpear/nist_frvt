#include "DescriptorsRecognizer.h"

namespace faces {

    bool DescriptorsRecognizer::save(std::string const &dst) {
        return classifier->save(dst);
    }

    bool DescriptorsRecognizer::_checkOk() {
        _ok = classifier->isOk() && descriptor->isOk();
        return _ok;
    }

    int DescriptorsRecognizer::_recognize(cv::Mat const &img) {
        if (!_checkOk()) return -2;

        std::vector<double> descriptors = descriptor->computeDescriptors(img);
        return classifier->classifyDescriptors(descriptors);
    }

    void DescriptorsRecognizer::train(std::map<int, cv::Mat &> const &samples) {
        if (!descriptor->isOk()) return;

        std::map<int, std::vector<double>> descriptorSamples;
        for (auto const &sample : samples) {
            descriptorSamples[sample.first] = descriptor->computeDescriptors(sample.second);
        }

        classifier->train(descriptorSamples);
        _checkOk();
    }

}