#ifndef FACES_DLIBRESNETDESCRIPTOR_H
#define FACES_DLIBRESNETDESCRIPTOR_H

#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/opencv.h>

#include <spdlog/spdlog.h>

#include <Config/Config.h>

#include "utils/utils.h"
#include <Recognizer/Descriptors/Descriptor.hpp>

namespace faces {

    /**
     * @see http://dlib.net/dnn_face_recognition_ex.cpp.html
     *
     * The next bit of code defines a ResNet network.  It's basically copied
     * and pasted from the dnn_imagenet_ex.cpp example, except we replaced the loss
     * layer with loss_metric and made the network somewhat smaller.  Go read the introductory
     * dlib DNN examples to learn what all this stuff means.
     *
     * Also, the dnn_metric_learning_on_images_ex.cpp example shows how to train this network.
     * The dlib_face_recognition_resnet_model_v1 model used by this example was trained using
     * essentially the code shown in dnn_metric_learning_on_images_ex.cpp except the
     * mini-batches were made larger (35x15 instead of 5x5), the iterations without progress
     * was set to 10000, and the training dataset consisted of about 3 million images instead of
     * 55.  Also, the input layer was locked to images of size 150.
     */
    namespace dlibResnet {
        template<template<int, template<typename> class, int, typename> class block, int N,
                template<typename> class BN, typename SUBNET>
        using Residual = dlib::add_prev1<block<N, BN, 1, dlib::tag1<SUBNET>>>;

        template<template<int, template<typename> class, int, typename> class block, int N,
                template<typename> class BN, typename SUBNET>
        using ResidualDown = dlib::add_prev2<dlib::avg_pool<2, 2, 2, 2,
                dlib::skip1<dlib::tag2<block<N, BN, 2, dlib::tag1<SUBNET>>>>>>;

        template<int N, template<typename> class BN, int stride, typename SUBNET>
        using Block  = BN<dlib::con<N, 3, 3, 1, 1, dlib::relu<BN<dlib::con<N, 3, 3, stride, stride, SUBNET>>>>>;

        template<int N, typename SUBNET> using Ares      = dlib::relu<Residual<Block, N, dlib::affine, SUBNET>>;
        template<int N, typename SUBNET> using AresDown = dlib::relu<ResidualDown<Block, N, dlib::affine, SUBNET>>;

        template<typename SUBNET> using Alevel0 = AresDown<256, SUBNET>;
        template<typename SUBNET> using Alevel1 = Ares<256, Ares<256, AresDown<256, SUBNET>>>;
        template<typename SUBNET> using Alevel2 = Ares<128, Ares<128, AresDown<128, SUBNET>>>;
        template<typename SUBNET> using Alevel3 = Ares<64, Ares<64, Ares<64, AresDown<64, SUBNET>>>>;
        template<typename SUBNET> using Alevel4 = Ares<32, Ares<32, Ares<32, SUBNET>>>;

        using AnetType = dlib::loss_metric<dlib::fc_no_bias<128, dlib::avg_pool_everything<
                Alevel0<
                        Alevel1<
                                Alevel2<
                                        Alevel3<
                                                Alevel4<
                                                        dlib::max_pool<3, 3, 2, 2,
                                                                dlib::relu<dlib::affine<dlib::con<
                                                                        32, 7, 7, 2, 2,
                                                                        dlib::input_rgb_image_sized<150>
                                                                >>>>>>>>>>>>;

        typedef dlib::matrix<double, 128, 1> DescriptorType;
    }


    /**
     * A descriptor based on the dlib`s ResNet network
     */
    class DlibResnetDescriptor : public Descriptor {
    public:
        FACES_MAIN_CONSTRUCTOR(explicit DlibResnetDescriptor, Config const &config);

        explicit DlibResnetDescriptor(std::string const &model);

    protected:
        FACES_OVERRIDE_ATTRIBUTE(faceSize, 150, 150)

        std::vector<double> _computeDescriptors(cv::Mat const &faceImg) override;

        /**
         * Deserializes descriptor net from the given .dat file
         *
         * @param src - a file, load a network from
         *
         * @return successfulness of the deserialization = current _ok
         */
        bool _load(std::string const &src);

    private:
        dlibResnet::AnetType _descriptor;

    };

    FACES_REGISTER_SUBCLASS(Descriptor, DlibResnetDescriptor, DlibResnet)

    FACES_AUGMENT_CONFIG(DlibResnetDescriptor,
                         FACES_ADD_CONFIG_OPTION("DlibResnetDescriptor.model", "mode", "", false,
                                                 "A path to a model file of Dlib-based resnet face descriptor"))

}

#endif //FACES_DLIBRESNETDESCRIPTOR_H
