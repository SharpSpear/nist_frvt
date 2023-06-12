#include <spdlog/sinks/stdout_color_sinks.h>

#include <miniconf.h>

#include <Face/Face.h>

#include <Config/Config.h>
#include <Detector/Implementations/OcvDefaultDnnDetector.h>
#include <Landmarker/Implementations/DlibLandmarker.h>
#include <Aligner/Implementations/DlibChipAligner.h>
#include <Recognizer/Implementations/Descriptors/DlibResnetSvmRecognizer.h>
#include <Tracker/Implementations/CentroidTracker.h>
#include <Database/DatabaseEntry.hpp>
#include <Database/Implementations/StandaloneDatabase.hpp>

namespace faces {
    FACES_AUGMENT_CONFIG(test,
                         FACES_ADD_CONFIG_OPTION("testVideo", "video", "", false,
                                                 "A video for testing"))
}

class FaceInfo : public faces::DatabaseEntry<FaceInfo> {
public:
    int a = 0;
    FACES_REGISTER_ACCESSOR(FaceInfo, a);

    float b = 0.0;
    FACES_REGISTER_ACCESSOR(FaceInfo, b);

    std::string c;
    FACES_REGISTER_ACCESSOR(FaceInfo, c);

    FaceInfo()
            : faces::DatabaseEntry<FaceInfo>(*this) {}

    explicit FaceInfo(std::map<std::string, std::any> const &attributes)
            : faces::DatabaseEntry<FaceInfo>(*this) {
        if (!initAttributes(attributes)) {
            throw std::logic_error("Cannot initialize attributes by the given map");
        }
    }
};

int main(int argc, char **argv) {
    auto console = spdlog::stdout_color_mt("console", spdlog::color_mode::always);
    spdlog::set_default_logger(console);

    faces::Config &configInstance = faces::Config::getInstance();
    miniconf::Config &config = configInstance.config;
    std::string configFile = FACES_ROOT_DIRECTORY "/config.json";
    bool configOk = config.config(configFile);
    if (!configOk) {
        spdlog::warn("Cannot load a config from the file '{}', saving a default configuration", configFile);
        config.serialize(configFile);
    }

    config.print();

    faces::StandaloneDatabase<FaceInfo> db(configInstance.getDataPath("facesDatabase"));
    if (!db.load()) {
        spdlog::warn("Failed to load a faces database");

        std::cout << "Save a new one? " << std::flush;
        bool shouldSave = false;
        std::cin >> shouldSave;
        if (shouldSave) {
            db.save();
        }
    }

    auto entryIds = db.getEntriesList();
    std::cout << "Database has the following entries:" << std::endl;
    for (auto const &entryId : entryIds) {
        FaceInfo e = db.get(entryId);
        std::cout << "\t" << std::flush;
        spdlog::info("Entry {}: a = {}; b = {}; c = {}", entryId, e.a, e.b, e.c);

        e.a *= 2;
        e.b += 1;
        db.update(entryId, e);
    }

    FaceInfo testE;
    testE.a = random() % 1024;
    testE.b = random() % 512;
    testE.c = std::to_string(testE.a + testE.b);
    db.add(testE);

    db.save();

    faces::Detector *detector = FACES_CREATE_INSTANCE(Detector, OcvDefaultDnn, configInstance);
    faces::Landmarker *landmarker = FACES_CREATE_INSTANCE(Landmarker, Dlib, configInstance);
    faces::Aligner *aligner = FACES_CREATE_INSTANCE(Aligner, DlibChip, configInstance);
    faces::Recognizer *recognizer = FACES_CREATE_INSTANCE(Recognizer, DlibResnetSvm, configInstance);
    faces::Tracker *tracker = FACES_CREATE_INSTANCE(Tracker, Centroid, configInstance);

    if (detector == nullptr || recognizer == nullptr || landmarker == nullptr || aligner == nullptr
        || tracker == nullptr) {
        spdlog::error("Cannot initialize some component!");
        return 1;
    }
    if (!detector->isOk() || !recognizer->isOk() || !landmarker->isOk() || !aligner->isOk()
        || !tracker->isOk()) {
        spdlog::error("Cannot load something!");
        return 1;
    }

    cv::VideoCapture cap(configInstance.getDataPath("testVideo"));
    cv::Mat test, prevTest;
    std::vector<faces::Face> detected, prevDetected;
    std::vector<std::pair<int, int>> tracked;

    while (cap.isOpened()) {
        cap >> test;

        detected = detector->detect(test);
        landmarker->detect(detected);
        aligner->align(detected, test);
        recognizer->recognize(detected);
        if (!prevTest.empty()) {
            tracked = tracker->track(prevDetected, detected, prevTest, test);
        }

        for (std::size_t i = 0; i < detected.size(); ++i) {
            faces::Face const &f = detected[i];

            cv::rectangle(test, f.rect, {0, 255, 0});
            cv::putText(test, std::to_string(f.label), f.rect.tl(),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, {255, 255, 255}, 2);

            for (cv::Point const &pt : f.landmarks) {
                cv::Point realPt = f.rect.tl() + pt;
                cv::circle(test, realPt, 2, {0, 0, 255}, cv::FILLED, cv::LINE_AA);
            }

            std::string faceWinName = std::to_string(i) + " " + std::to_string(f.label);
            cv::namedWindow(faceWinName, cv::WINDOW_GUI_NORMAL | cv::WINDOW_AUTOSIZE);
            cv::imshow(faceWinName, f.img);

            // std::cout << f.rect << " " << f.label << std::endl;
        }

        for (auto const &trackedPair : tracked) {
            int const &prev = trackedPair.first;
            int const &actual = trackedPair.second;
            if (prev != -1 && actual != -1) {
                cv::Point prevCenter = (prevDetected[prev].rect.br() + prevDetected[prev].rect.tl()) / 2;
                cv::Point actualCenter = (detected[actual].rect.br() + detected[actual].rect.tl()) / 2;
                cv::line(test, prevCenter, actualCenter, {255, 0, 0}, 2);
            } else {
                int nonMatchedIdx = std::max(prev, actual);
                std::vector<faces::Face> const &nonMatchedFrom = prev == -1 ? detected : prevDetected;
                faces::Face const &nonMatchedFace = nonMatchedFrom[nonMatchedIdx];
                cv::Point nonMatchedCenter = (nonMatchedFace.rect.br() + nonMatchedFace.rect.tl()) / 2;

                cv::circle(test, nonMatchedCenter, 5, {0, 255, 255}, cv::FILLED);
            }
        }

        prevDetected = detected;
        prevTest = test;

        cv::imshow("test", test);
        cv::waitKey(1);
    }

    return 0;
}