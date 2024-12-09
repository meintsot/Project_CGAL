# Created by the script cgal_create_CMakeLists
# This is the CMake script for compiling a set of CGAL applications.

cmake_minimum_required(VERSION 3.1...3.23)

project(opt_triangulation)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set CGAL_DIR if CGAL is installed in a non-standard location
#set(CGAL_DIR "/home/`01/Desktop/CGAL-5.6.1/install/lib/cmake/CGAL")
set(CGAL_DIR "/usr/local/lib/cmake/CGAL")



# CGAL and its components
find_package(CGAL QUIET COMPONENTS Core)
find_package(CGAL COMPONENTS Qt5)


if(NOT CGAL_FOUND)
  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()
endif()

# Boost and its components
find_package(Boost REQUIRED)

if(NOT Boost_FOUND)
  message(STATUS "This project requires the Boost library, and will not be compiled.")
  return()
endif()

# Find GMP and MPFR (CGAL dependencies)
find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)

if(NOT GMP_FOUND)
  message(FATAL_ERROR "GMP library not found.")
endif()

if(NOT MPFR_FOUND)
  message(FATAL_ERROR "MPFR library not found.")
endif()

# Include directories
include_directories(
  ${CGAL_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR}
  ${MPFR_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/include       # For your project's headers
  /usr/include/eigen3               # For Eigen
)

# Add definitions
add_definitions(${CGAL_DEFINITIONS})

# Source files
set(SRC_FILES
  src/main.cpp
  src/jsonUtils.cpp
  src/triangulationUtils.cpp
  src/circumCenterMethod.cpp
  src/midpointMethod.cpp
  src/centroidMethod.cpp
  src/oneCentroid.cpp
  src/projectionMethod.cpp
)

# Creating entries for target: opt_triangulation
# ##########################################
add_executable(opt_triangulation ${SRC_FILES})

# Link the executable to CGAL and third-party libraries
target_link_libraries(opt_triangulation PRIVATE
  CGAL::CGAL
  ${Boost_LIBRARIES}
  ${GMP_LIBRARIES}
  ${MPFR_LIBRARIES}
)

#qt testing
add_definitions(-DCGAL_USE_BASIC_VIEWER)
target_link_libraries(opt_triangulation PUBLIC CGAL::CGAL_Qt5)