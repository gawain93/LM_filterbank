cmake_minimum_required(VERSION 2.8)
project(test_proposal)

include_directories(include)
set(SOURCES src/filter_bank.cpp
            src/texture_recog.cpp
            src/compute_histogram.cpp)
find_package(OpenCV REQUIRED)

add_executable(test_proposal main.cpp
                             src/filter_bank.cpp
                             src/texture_recog.cpp
                             src/compute_histogram.cpp)
target_link_libraries(test_proposal ${OpenCV_LIBS})
install(TARGETS test_proposal RUNTIME DESTINATION bin)
