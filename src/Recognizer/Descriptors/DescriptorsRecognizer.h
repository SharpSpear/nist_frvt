#ifndef FACES_DESCRIPTORSRECOGNIZER_H
#define FACES_DESCRIPTORSRECOGNIZER_H

#include <type_traits>

#include "../Recognizer.hpp"

#include "Descriptor.hpp"
#include "DescriptorsClassifier.hpp"

namespace faces {

    /**
     * A base class for all of the descriptor-based face recognizers, which allows to
     * just specify descriptor and classifier in the derived class without overriding any methods
     */
    class DescriptorsRecognizer : public Recognizer {
    public:
        /// these fields should be initialized in the derived class` constructor

        DescriptorsClassifier *classifier;
        Descriptor *descriptor;

        /**
         * Saves the classifier, by calling its `save` method
         *
         * @param dst - destination, save a classifier to;
         *              it may be a file or a directory, depends on the selected classifier
         *
         * @return successfulness of the save
         */
        bool save(std::string const &dst);

        /**
         * Trains a classifier by generating descriptors for each face image in a `samples` map;
         * It only works when the `detector` is `ok`, but it does not check the status of the classifier
         *
         * @param samples - a map in a format {label: face ROI}
         */
        void train(std::map<int, cv::Mat &> const &samples) override;

    protected:
        /**
         * Sets this class` `_ok` value base on states of `descriptor` and `classifier`
         *
         * @return the current `_ok` value
         */
        bool _checkOk();

        /**
         * Estimates a label by generating a descriptor for the given face image and classifying it;
         * It works only when both `descriptor` and `classifier` are ok
         *
         * @param img - a photo of the face
         *
         * @returns a label of the face
         */
        int _recognize(cv::Mat const &img) override;
    };

}

#endif //FACES_DESCRIPTORSRECOGNIZER_H
