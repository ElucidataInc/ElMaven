# Install script for directory: /home/melamud/Downloads/eigen/Eigen

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "/usr/local/include/eigen2/Eigen" TYPE FILE FILES
    "/home/melamud/Downloads/eigen/Eigen/Core"
    "/home/melamud/Downloads/eigen/Eigen/LU"
    "/home/melamud/Downloads/eigen/Eigen/Cholesky"
    "/home/melamud/Downloads/eigen/Eigen/QR"
    "/home/melamud/Downloads/eigen/Eigen/Geometry"
    "/home/melamud/Downloads/eigen/Eigen/Sparse"
    "/home/melamud/Downloads/eigen/Eigen/Array"
    "/home/melamud/Downloads/eigen/Eigen/SVD"
    "/home/melamud/Downloads/eigen/Eigen/LeastSquares"
    "/home/melamud/Downloads/eigen/Eigen/QtAlignedMalloc"
    "/home/melamud/Downloads/eigen/Eigen/StdVector"
    "/home/melamud/Downloads/eigen/Eigen/NewStdVector"
    "/home/melamud/Downloads/eigen/Eigen/Eigen"
    "/home/melamud/Downloads/eigen/Eigen/Dense"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/melamud/Downloads/eigen/Eigen/src/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

