target_sources(faces
        PRIVATE
        PUBLIC
        Recognizer.hpp
        )

add_subdirectory(Descriptors)
add_subdirectory(Implementations)