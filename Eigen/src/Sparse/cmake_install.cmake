# Install script for directory: /home/melamud/Downloads/eigen/Eigen/src/Sparse

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

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Devel")
  FILE(INSTALL DESTINATION "/usr/local/include/eigen2/Eigen/src/Sparse" TYPE FILE FILES
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseRedux.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseDot.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseFuzzy.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseAssign.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/AmbiVector.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseCwise.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/CompressedStorage.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseLU.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseCwiseUnaryOp.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseLLT.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/CoreIterators.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/MappedSparseMatrix.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseMatrixBase.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/TaucsSupport.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SuperLUSupport.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseFlagged.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseMatrix.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseProduct.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/CholmodSupport.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseBlock.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseVector.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseUtil.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/RandomSetter.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseDiagonalProduct.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/DynamicSparseMatrix.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/TriangularSolver.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/UmfPackSupport.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseLDLT.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseTranspose.h"
    "/home/melamud/Downloads/eigen/Eigen/src/Sparse/SparseCwiseBinaryOp.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Devel")

