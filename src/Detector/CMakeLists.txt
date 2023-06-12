target_sources(faces
        PRIVATE
        OcvDnnDetector.cpp
        PUBLIC
        Detector.hpp
        OcvDnnDetector.h
        )

add_subdirectory(Implementations)