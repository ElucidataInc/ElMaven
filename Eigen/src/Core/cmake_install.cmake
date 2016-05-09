# Install script for directory: /home/melamud/Downloads/eigen/Eigen/src/Core

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
  FILE(INSTALL DESTINATION "/usr/local/include/eigen2/Eigen/src/Core" TYPE FILE FILES
    "/home/melamud/Downloads/eigen/Eigen/src/Core/MapBase.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/CommaInitializer.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Dot.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Matrix.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Map.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Block.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Cwise.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/CwiseUnaryOp.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Minor.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Flagged.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/GenericPacketMath.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Sum.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/MatrixBase.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/NumTraits.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/CwiseNullaryOp.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Transpose.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Coeffs.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/NestByValue.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/IO.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Visitor.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Redux.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/MatrixStorage.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/CwiseBinaryOp.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Swap.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/SolveTriangular.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Product.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/DiagonalMatrix.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Functors.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Assign.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Part.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/DiagonalProduct.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/DiagonalCoeffs.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/Fuzzy.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/CacheFriendlyProduct.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Core/MathFunctions.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/melamud/Downloads/eigen/Eigen/src/Core/util/cmake_install.cmake")
  INCLUDE("/home/melamud/Downloads/eigen/Eigen/src/Core/arch/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

