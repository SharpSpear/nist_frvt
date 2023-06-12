#ifndef FACES_CENTROIDTRACKER_H
#define FACES_CENTROIDTRACKER_H

#include <utils/utils.h>
#include <Config/Config.h>

#include <Tracker/Tracker.hpp>

namespace faces {

    /**
     * A nearest centroid tracker, which matches face to the one with the nearest center point
     */
    class CentroidTracker : public Tracker {
    public:
        FACES_MAIN_CONSTRUCTOR(explicit CentroidTracker, Config const &config);

    protected:
        int _maxDistance;

        std::vector<std::pair<int, int>> _track(std::vector<Face> const &prevFaces,
                                                std::vector<Face> const &actualFaces,
                                                cv::Mat const &prevImg, cv::Mat const &actualImg) override;

        /**
         * @return a vector of center points of the given faces
         */
        [[nodiscard]] static std::vector<cv::Point> _getCentroids(std::vector<Face> const &faces);

    };

    FACES_REGISTER_SUBCLASS(Tracker, CentroidTracker, Centroid)

    FACES_AUGMENT_CONFIG(CentroidTracker,
                         FACES_ADD_CONFIG_OPTION("CentroidTracker.maxDistance", "maxDistance", 150,
                                                 false, "A maximum distance between two matched faces")
    )

}


#endif //FACES_CENTROIDTRACKER_H
