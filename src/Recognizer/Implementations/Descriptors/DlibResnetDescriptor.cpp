#include "DlibResnetDescriptor.h"

namespace faces {

    DlibResnetDescriptor::DlibResnetDescriptor(Config const &config) {
        std::string const &model = config.getModelPath("DlibResnetDescriptor.model");
        _load(model);
    }

    DlibResnetDescriptor::DlibResnetDescriptor(std::string const &model) {
        _load(model);
    }

    std::vector<double> DlibResnetDescriptor::_computeDescriptors(cv::Mat const &faceImg) {
        dlib::cv_image<dlib::rgb_pixel> dFaceImg;
        cv::Mat rgbImg;
        cv::cvtColor(faceImg, rgbImg, cv::COLOR_BGR2RGB);
        dFaceImg = dlib::cv_image<dlib::rgb_pixel>(rgbImg);

        std::vector<dlib::matrix<float, 0, 1>> faceDescriptorsF =
                _descriptor(std::vector<dlib::cv_image<dlib::rgb_pixel>>{dFaceImg});
        std::vector<dlibResnet::DescriptorType> faceDescriptors;
        for (auto &desc : faceDescriptorsF) {
            std::vector<double> descVec;
            for (unsigned int r = 0; r < desc.nr(); r += 1) {
                descVec.emplace_back(desc(r, 0));
            }
            faceDescriptors.emplace_back(dlib::mat(descVec));
        }

        dlibResnet::DescriptorType descriptor = faceDescriptors[0];

        return std::vector<double>(descriptor.begin(), descriptor.end());
    }

    bool DlibResnetDescriptor::_load(std::string const &src) {
        try {
            dlib::deserialize(src) >> _descriptor;
            _ok = true;
        } catch (dlib::serialization_error &e) {
            spdlog::error("Cannot load a dlib face descriptor from {}: {}", src, e.what());
            _ok = false;
        }
        return _ok;
    }

}
