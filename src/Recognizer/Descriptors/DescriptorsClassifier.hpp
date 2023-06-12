#ifndef FACES_DESCRIPTORSCLASSIFIER_HPP
#define FACES_DESCRIPTORSCLASSIFIER_HPP

#include <vector>
#include <map>

#include "utils/utils.h"

namespace faces {

    /**
     * A base class for all of the descriptor classifiers
     */
    class DescriptorsClassifier {
    public:
        /// a confidence threshold for label predictions;
        /// all the predictions bellow should be treated as unrecognized
        FACES_DECLARE_ATTRIBUTE(double, threshold)

        /**
         * Classifies the given face descriptor
         * It is just a wrapper around @ref _classifyDescriptors, which check `_ok`
         *
         * @param descriptors - face descriptors
         *
         * @return an estimated label OR -2 if classifier is not `_ok`
         */
        int classifyDescriptors(const std::vector<double> &descriptors) {
            if (!_ok) {
                return -2;
            }

            return _classifyDescriptors(descriptors);
        }

        /**
         * Trains the classifier on the given samples
         *
         * @param samples - a map in format {true label: descriptors}
         */
        virtual void train(std::map<int, std::vector<double>> const &samples) = 0;

        /**
         * Saves the classifier to the given destination
         * It is just a wrapper around @ref _save, which check `_ok`
         *
         * @param dst - a file or directory where to save a classifier
         *
         * @return successfulness of saving OR `false` if classifier is not `_ok`
         */
        bool save(std::string const &dst) {
            if (!_ok) {
                return false;
            }

            return _save(dst);
        }

        /**
         * @return a value of the @ref _ok flag
         */
        [[nodiscard]] bool isOk() const {
            return _ok;
        }

    protected:
        /// the flag which indicates the readiness of the detector
        bool _ok = false;

        /**
         * Classifies the given face descriptor
         *
         * @param descriptors - face descriptors
         *
         * @return an estimated label
         */
        virtual int _classifyDescriptors(const std::vector<double> &descriptors) = 0;

        /**
         * Saves the classifier to the given destination
         *
         * @param dst - a file or directory where to save a classifier
         *
         * @return successfulness of saving
         */
        virtual bool _save(std::string const &dst) = 0;

    };

}

#endif //FACES_DESCRIPTORSCLASSIFIER_HPP
