cmake_minimum_required(VERSION 3.16.5)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 20)
set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/bin)

# set the project name
project(spline_solver)

# Use OpenCV and Eigen
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(includes/ ${EIGEN3_INCLUDE_DIR} ${OpenCV_INCLUDE_DIR})

# add lib
add_library(algo STATIC src/algo/hermite_spline.cpp src/algo/b_spline.cpp)

# add the executable
add_executable(bspline src/examples/b_spline.cpp)
add_dependencies(bspline algo)

# link target
target_link_libraries(bspline algo ${OpenCV_LIBS} ${Eigen3_LIBS})

# turn optimization on
target_compile_options(algo PRIVATE -O3)

# make sure install directory exsists
file(MAKE_DIRECTORY ${INSTALL_DIR})
file(MAKE_DIRECTORY ${INSTALL_DIR}/examples)

# installation
install(TARGETS algo DESTINATION ${INSTALL_DIR})
install(TARGETS bspline DESTINATION ${INSTALL_DIR}/examples)