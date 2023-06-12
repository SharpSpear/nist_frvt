#include "DlibSvmClassifier.h"
#include "DlibResnetDescriptor.h"

faces::DlibSvmClassifier::DlibSvmClassifier(Config const &config) {
    std::string const &classifiersFile = config.getDataPath("DlibSvmClassifier.classifiers");
    _load(classifiersFile);
}

faces::DlibSvmClassifier::DlibSvmClassifier(std::string const &classifiersFile) {
    _load(classifiersFile);
}

void faces::DlibSvmClassifier::train(std::map<int, std::vector<double>> const &samples) {
    std::vector<dlibResnet::DescriptorType> descriptors;
    std::vector<int> labels;

    for (auto const &sample : samples) {
        labels.emplace_back(sample.first);
        descriptors.emplace_back(dlib::mat(sample.second));
    }

    // Unique labels ->
    std::vector<int> totalLabels;
    for (int &label : labels) {
        if (find(totalLabels.begin(), totalLabels.end(), label) == totalLabels.end())
            totalLabels.emplace_back(label);
    }
    // <- Unique labels

    // Init trainers ->
    std::vector<dlibSvm::TrainerType> trainers;
    unsigned long numTrainers = totalLabels.size() * (totalLabels.size() - 1) / 2;

    for (int i = 0; i < numTrainers; i++) {
        trainers.emplace_back(dlibSvm::TrainerType());
        trainers[i].set_kernel(dlibSvm::KernelType());
        trainers[i].set_c(10);
    }
    // <- Init trainers

    // Train and init classifiers ->
    int label1 = 0, label2 = 1;
    for (dlibSvm::TrainerType &trainer : trainers) {
        std::vector<dlibResnet::DescriptorType> samples4Pair;
        std::vector<double> labels4Pair;

        for (int i = 0; i < descriptors.size(); i++) {
            if (labels[i] == totalLabels[label1]) {
                samples4Pair.emplace_back(descriptors[i]);
                labels4Pair.emplace_back(-1);
            }
            if (labels[i] == totalLabels[label2]) {
                samples4Pair.emplace_back(descriptors[i]);
                labels4Pair.emplace_back(+1);
            }
        }

        _classifiers.emplace_back(totalLabels[label1],
                                  totalLabels[label2],
                                  trainer.train(samples4Pair, labels4Pair));

        label2++;
        if (label2 == totalLabels.size()) {
            label1++;
            label2 = label1 + 1;
        }
    }
    // <- Train and init classifiers

    _ok = !_classifiers.empty();
}

int faces::DlibSvmClassifier::_classifyDescriptors(const std::vector<double> &descriptors) {
    std::map<double, int> votes;
    for (auto &faceClassifier : _classifiers) {
        dlibResnet::DescriptorType dlibDescriptors = dlib::mat(descriptors);
        double prediction = faceClassifier.classify(dlibDescriptors, get_threshold());
        if (prediction == -1)
            continue;
        votes[prediction]++;
    }

    auto max = std::max_element(votes.begin(), votes.end(),
                                [](const std::pair<int, int> &p1, const std::pair<int, int> &p2) {
                                    return p1.second < p2.second;
                                });

    int label = votes.empty() ? -1 : max->first;
    return label;
}

bool faces::DlibSvmClassifier::_save(std::string const &dst) {
    try {
        dlib::serialize(dst) << _classifiers;
        return true;
    } catch (dlib::serialization_error &e) {
        spdlog::error("Cannot save descriptor classifiers to {}: {}", dst, e.what());
        return false;
    }
}

bool faces::DlibSvmClassifier::_load(std::string const &classifiersFile) {
    try {
        dlib::deserialize(classifiersFile) >> _classifiers;
        _ok = true;
        if (_classifiers.empty()) {
            spdlog::error("Descriptor classifiers were loaded without errors from file {}, "
                          "however they are empty!", classifiersFile);
            _ok = false;
        }
    } catch (dlib::serialization_error &e) {
        spdlog::error("Cannot load descriptor classifiers from {}: {}", classifiersFile, e.what());
        _ok = false;
    }
    return _ok;
}
