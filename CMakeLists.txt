cmake_minimum_required(VERSION 2.8)
project(test_proposal)

include_directories(include)
set(SOURCES src/filter_bank.cpp
            src/texture_recog.cpp
            src/compute_histogram.cpp
            src/image_blend.cpp
            src/segment_dirt.cpp)
find_package(OpenCV REQUIRED)
FIND_PACKAGE( Boost REQUIRED COMPONENTS filesystem system)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable(test_proposal main.cpp
                             src/filter_bank.cpp
                             src/texture_recog.cpp
                             src/compute_histogram.cpp
                             src/image_blend.cpp
                             src/segment_dirt.cpp)
                             
#add_executable(dataset_create dataset_main.cpp
#                            src/image_blend.cpp)

target_link_libraries(test_proposal ${OpenCV_LIBS} ${Boost_LIBRARIES})
install(TARGETS test_proposal RUNTIME DESTINATION bin)
