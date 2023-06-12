#ifndef FACES_TRACKER_HPP
#define FACES_TRACKER_HPP

#include <Face/Face.h>

namespace faces {

    // TODO: allow creating non-frame2frame trackers
    //       i.e. trackers which do not require running detector for every frame

    /**
     * A base class for all of the face trackers
     */
    class Tracker {
    public:
        /**
         * Tracks faces frame-to-frame. @n
         * This is a wrapper around the @ref _track method
         *
         * @param prevFaces     - a vector of faces on the previous frame
         * @param actualFaces   - a vector of faces on the current frame
         * @param prevImg       - a previous image where @p prevFaces were detected
         * @param actualImg     - a current image where @p actualFaces were detected
         *
         * @return a vector of pairs of matching face indexes - {previousIdx, actualIdx};
         *         if a face does not have a match, its pair should be set to '-1'
         */
        std::vector<std::pair<int, int>> track(std::vector<Face> const &prevFaces,
                                               std::vector<Face> const &actualFaces,
                                               cv::Mat const &prevImg, cv::Mat const &actualImg) {
            if (!_ok) {
                return {};
            }

            return _track(prevFaces, actualFaces, prevImg, actualImg);
        }

        /**
         * @return a value of the @ref _ok flag
         */
        [[nodiscard]] bool isOk() const {
            return _ok;
        }

    protected:
        /// the flag which indicates the readiness of the recognizer
        bool _ok = false;

        /**
         * Tracks faces frame-to-frame
         *
         * @param prevFaces     - a vector of faces on the previous frame
         * @param actualFaces   - a vector of faces on the current frame
         * @param prevImg       - a previous image where @p prevFaces were detected
         * @param actualImg     - a current image where @p actualFaces were detected
         *
         * @return a vector of pairs of matching face indexes - {previousIdx, actualIdx};
         *         if a face does not have a match, its pair should be set to '-1'
         */
        virtual std::vector<std::pair<int, int>> _track(std::vector<Face> const &prevFaces,
                                                        std::vector<Face> const &actualFaces,
                                                        cv::Mat const &prevImg, cv::Mat const &actualImg) = 0;

    };

}

#endif //FACES_TRACKER_HPP
