#ifndef FACES_DLIBSVMCLASSIFIER_H
#define FACES_DLIBSVMCLASSIFIER_H

#include "DlibResnetDescriptor.h"
#include <Recognizer/Descriptors/DescriptorsClassifier.hpp>

namespace faces {

    /**
     * This namespace contains a wrapper around dlib`s decision_function
     * and (de)serialization functions for it
     */
    namespace dlibSvm {
        using KernelType = dlib::histogram_intersection_kernel<dlibResnet::DescriptorType>;
        using TrainerType =  dlib::svm_c_trainer<KernelType>;

        /**
         * A wrapper around dlib`s decision_function which compares the prediction with threshold
         * and returns specified labels for negative and positive predictions or -1
         */
        class SingleSvmClassifier {
        public:
            int negativeLabel = -1;
            int positiveLabel = -1;

            dlib::decision_function<KernelType> classifier;

            SingleSvmClassifier() = default;

            SingleSvmClassifier(int negativeLabel, int positiveLabel,
                                dlib::decision_function<KernelType> const &classifier)
                    : negativeLabel(negativeLabel), positiveLabel(positiveLabel),
                      classifier(classifier) {}

            double classify(dlibResnet::DescriptorType const &descriptor, double threshold = 0.3) {
                double prediction = classifier(descriptor);
                if (fabs(prediction) < threshold)
                    return -1;
                if (prediction < 0)
                    return negativeLabel;
                return positiveLabel;
            }

        };

        inline void serialize(const dlibSvm::SingleSvmClassifier &item, std::ostream &out) {
            try {
                dlib::serialize(item.negativeLabel, out);
                dlib::serialize(item.positiveLabel, out);
                dlib::serialize(item.classifier, out);
            } catch (dlib::serialization_error &e) {
                throw dlib::serialization_error(
                        e.info + "\n while serializing an object of type MyClassifier");
            }
        }

        inline void deserialize(dlibSvm::SingleSvmClassifier &item, std::istream &in) {
            try {
                dlib::deserialize(item.negativeLabel, in);
                dlib::deserialize(item.positiveLabel, in);
                dlib::deserialize(item.classifier, in);
            } catch (dlib::serialization_error &e) {
                throw dlib::serialization_error(
                        e.info + "\n while deserializing an object of type MyClassifier");
            }
        }
    }

    /**
     * An implementation of descriptors classifier based on one vs one SVM classifier
     */
    class DlibSvmClassifier : public DescriptorsClassifier {
    public:
        FACES_OVERRIDE_ATTRIBUTE(threshold, 0.3)

        FACES_MAIN_CONSTRUCTOR(explicit DlibSvmClassifier, Config const &config);

        explicit DlibSvmClassifier(std::string const &classifiersFile);

        void train(std::map<int, std::vector<double>> const &samples) override;

    protected:
        int _classifyDescriptors(const std::vector<double> &descriptors) override;

        bool _save(std::string const &dst) override;

        /**
         * Deserializes classifiers from the given file
         *
         * @param classifiersFile - a file with classifiers, created in the @ref _save method
         *
         * @return successfulness of the deserialization = current _ok
         */
        bool _load(std::string const &classifiersFile);

    private:
        std::vector<dlibSvm::SingleSvmClassifier> _classifiers;

    };

    FACES_REGISTER_SUBCLASS(DescriptorsClassifier, DlibSvmClassifier, DlibSvm)

    FACES_AUGMENT_CONFIG(DlibSvmClassifier,
                         FACES_ADD_CONFIG_OPTION("DlibSvmClassifier.classifiers", "classifiers", "", false,
                                                 "A path to a model file of Dlib-based SVM OvO face descriptor classifier"))

}

#endif //FACES_DLIBSVMCLASSIFIER_H
