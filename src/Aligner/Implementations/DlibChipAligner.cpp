#include "DlibChipAligner.h"

namespace faces {

    DlibChipAligner::DlibChipAligner(const Config &config) : Aligner(config) {
        _ok = true;
    }

    cv::Mat DlibChipAligner::_align(Face const &face, cv::Mat const &wholeImg) {
        assert(!face.landmarks.empty() && "Face does not have landmarks, but this aligner requires them!");

        dlib::cv_image<dlib::bgr_pixel> dImg = wholeImg;

        dlib::full_object_detection shape(dRect(face.rect), dPointsVec(face.getRectLandmarks()));
        dlib::matrix<dlib::rgb_pixel> faceChip;
        dlib::chip_details faceChipDet = get_face_chip_details(shape, _faceSize.width, 0.25);
        dlib::extract_image_chip(dImg, faceChipDet, faceChip);

        return dlibMatrix2CvMat(faceChip);
    }

}