cmake_minimum_required(VERSION 3.16.5)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 17)
set(OPTIM_FLAGS -funroll-loops -O3 -march=native -mfpmath=sse -mavx -mfma)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# set the project name
project(spline_solver)

# Use OpenCV and Eigen
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(SigC++ REQUIRED)
find_package(Cairo REQUIRED)
find_package(Cairomm REQUIRED)
find_package(OpenMP REQUIRED)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


include_directories(includes/ ${Cairomm_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} ${OpenCV_INCLUDE_DIR})

# add lib
add_library(algo STATIC
    src/algo/hermite_spline.cpp 
    src/algo/b_spline.cpp 
    src/algo/noise.cpp 
    src/algo/generative.cpp)

# add the executable
add_executable(bspline src/examples/b_spline.cpp)
add_dependencies(bspline algo)

add_executable(perlin_noise src/examples/perlin_noise.cpp)
add_dependencies(perlin_noise algo)

add_executable(circle src/examples/circle.cpp)
add_dependencies(circle algo)

add_executable(sand src/examples/sand.cpp)
add_dependencies(sand algo)

# link target
target_link_libraries(algo ${OpenCV_LIBS} ${Eigen3_LIBS} ${Cairomm_LIBRARIES} OpenMP::OpenMP_CXX)
target_link_libraries(bspline algo ${OpenCV_LIBS} ${Eigen3_LIBS})
target_link_libraries(perlin_noise algo ${OpenCV_LIBS} ${Eigen3_LIBS})
target_link_libraries(circle algo ${OpenCV_LIBS} ${Eigen3_LIBS})
target_link_libraries(sand algo ${OpenCV_LIBS} ${Eigen3_LIBS} ${Cairomm_LIBRARIES} ${CAIRO_LIBRARIES} OpenMP::OpenMP_CXX)

# turn optimization on
target_compile_options(algo PRIVATE ${OPTIM_FLAGS})
target_compile_options(perlin_noise PRIVATE ${OPTIM_FLAGS})
target_compile_options(circle PRIVATE ${OPTIM_FLAGS})
target_compile_options(sand PRIVATE ${OPTIM_FLAGS})
