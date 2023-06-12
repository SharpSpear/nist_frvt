#include <numeric>
#include "CentroidTracker.h"

namespace faces {

    CentroidTracker::CentroidTracker(Config const &config) {
        _ok = true;

        try {
            _maxDistance = config["CentroidTracker.maxDistance"].getInt();
        } catch (std::out_of_range &e) {
            spdlog::error("Cannot get am option 'CentroidTracker.maxDistance' in the config!");
            _maxDistance = INT_MAX;
        }

    }

    std::vector<std::pair<int, int>> CentroidTracker::_track(std::vector<Face> const &prevFaces,
                                                             std::vector<Face> const &actualFaces,
                                                             cv::Mat const &prevImg,
                                                             cv::Mat const &actualImg) {
        std::vector<std::pair<int, int>> res;
        std::map<std::size_t, std::pair<std::size_t, double>> candidates;

        std::vector<cv::Point> prevCentroids = _getCentroids(prevFaces);
        std::vector<cv::Point> actualCentroids = _getCentroids(actualFaces);

        for (std::size_t i = 0; i < prevCentroids.size(); ++i) {
            std::vector<double> distances;
            std::transform(actualCentroids.begin(), actualCentroids.end(), std::back_inserter(distances),
                           [&](cv::Point &pt) -> double { return getDist(prevCentroids[i], pt); });

            auto minDistance = std::min_element(distances.begin(), distances.end());
            if (!distances.empty()) {
                int idx = std::distance(distances.begin(), minDistance);

                decltype(candidates)::iterator candidateItr;
                bool isFoundAndGreater = (candidateItr = candidates.find(idx)) != candidates.end()
                                         && *minDistance < candidateItr->second.second;
                if (*minDistance < _maxDistance && (isFoundAndGreater || candidateItr == candidates.end())) {
                    auto &candidate = candidates[idx];
                    candidate.first = i;
                    candidate.second = *minDistance;
                }
            }
        }

        for (auto const &candidate : candidates) {
            std::size_t const &actualIdx = candidate.first;
            std::size_t const &prevIdx = candidate.second.first;
            res.emplace_back(prevIdx, actualIdx);

            prevCentroids[prevIdx].x = -1;
            actualCentroids[actualIdx].x = -1;
        }

        for (std::size_t i = 0; i < prevCentroids.size(); ++i) {
            if (prevCentroids[i].x != -1) {
                res.emplace_back(-1, i);
            }
        }
        for (std::size_t i = 0; i < actualCentroids.size(); ++i) {
            if (actualCentroids[i].x != -1) {
                res.emplace_back(i, -1);
            }
        }

        return res;
    }

    std::vector<cv::Point> CentroidTracker::_getCentroids(std::vector<Face> const &faces) {
        std::vector<cv::Point> res;
        std::transform(faces.begin(), faces.end(), std::back_inserter(res),
                       [](Face const &face) -> cv::Point { return (face.rect.br() + face.rect.tl()) / 2; });
        return res;
    }


}