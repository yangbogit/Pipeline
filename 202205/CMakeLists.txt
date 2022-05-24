# 指定 Cmake 版本
cmake_minimum_required(VERSION 3.22)

project(Pipeline)
set(CMAKE_CXX_STANDARD 14)
add_executable(Pipeline src/PipelineMain.cpp)
add_executable(Serial src/SerialMain.cpp)

# 设置 OpenCV
set(OpenCV_DIR C:/software/software1/OpenCV/opencv/MinGW-build/install)
find_package(OpenCV REQUIRED)
target_link_libraries(Pipeline ${OpenCV_LIBS})
target_link_libraries(Serial ${OpenCV_LIBS})