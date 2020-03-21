cmake_minimum_required(VERSION 3.16.5)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 20)
set(OPTIM_FLAGS -funroll-loops -O3 -march=native -mfpmath=sse -mavx -mfma -mzvector)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set the project name
project(spline_solver)

# Use OpenCV and Eigen
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(includes/ ${EIGEN3_INCLUDE_DIR} ${OpenCV_INCLUDE_DIR})

# add lib
add_library(algo STATIC src/algo/hermite_spline.cpp src/algo/b_spline.cpp src/algo/noise.cpp)

# add the executable
add_executable(bspline src/examples/b_spline.cpp)
add_dependencies(bspline algo)

add_executable(perlin_noise src/examples/perlin_noise.cpp)
add_dependencies(perlin_noise algo)

# link target
target_link_libraries(bspline algo ${OpenCV_LIBS} ${Eigen3_LIBS})
target_link_libraries(perlin_noise algo ${OpenCV_LIBS} ${Eigen3_LIBS})

# turn optimization on
target_compile_options(algo PRIVATE ${OPTIM_FLAGS})
target_compile_options(perlin_noise PRIVATE ${OPTIM_FLAGS})
