###############################################################################
#
# src/CMakeLists.txt controls building of SIDX library
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#
###############################################################################

###############################################################################
# Source files specification
#
# Naming format:
#   SIDX_CPP - all the .cpp files
#   SIDX_HPP - all the .hpp files
#   SIDX_<dirname>_CPP - all the .cpp files for the given subdir/namespace
#   ...

set(SIDX_CPP "")

set(SIDX_SRC_DIR "${PROJECT_SOURCE_DIR}/src")

#
# base
#

set(SIDX_SPATIALINDEX_CPP
    "${SIDX_SRC_DIR}/spatialindex/IdVisitor.cc"
    "${SIDX_SRC_DIR}/spatialindex/IdVisitor.h"
    "${SIDX_SRC_DIR}/spatialindex/IndexTool.h"
    "${SIDX_SRC_DIR}/spatialindex/LineSegment.cc"
    "${SIDX_SRC_DIR}/spatialindex/LineSegment.h"
    "${SIDX_SRC_DIR}/spatialindex/Point.cc"
    "${SIDX_SRC_DIR}/spatialindex/Point.h"
    "${SIDX_SRC_DIR}/spatialindex/Region.cc"
    "${SIDX_SRC_DIR}/spatialindex/Region.h"
    "${SIDX_SRC_DIR}/spatialindex/SpatialIndex.cc"
    "${SIDX_SRC_DIR}/spatialindex/SpatialIndex.h"
    )
list(APPEND SIDX_CPP ${SIDX_SPATIALINDEX_CPP})

set(SIDX_RTREE_CPP
    "${SIDX_SRC_DIR}/rtree/BulkLoader.cc"
    "${SIDX_SRC_DIR}/rtree/BulkLoader.h"
    "${SIDX_SRC_DIR}/rtree/Index.cc"
    "${SIDX_SRC_DIR}/rtree/Index.h"
    "${SIDX_SRC_DIR}/rtree/Leaf.cc"
    "${SIDX_SRC_DIR}/rtree/Leaf.h"
    "${SIDX_SRC_DIR}/rtree/Node.cc"
    "${SIDX_SRC_DIR}/rtree/Node.h"
    "${SIDX_SRC_DIR}/rtree/PointerPoolNode.h"
    "${SIDX_SRC_DIR}/rtree/RTree.cc"
    "${SIDX_SRC_DIR}/rtree/RTree.h"
    "${SIDX_SRC_DIR}/rtree/Statistics.cc"
    "${SIDX_SRC_DIR}/rtree/Statistics.h"
    )
list(APPEND SIDX_CPP ${SIDX_RTREE_CPP})

set(SIDX_STORAGEMANAGER_CPP
    "${SIDX_SRC_DIR}/storagemanager/Buffer.cc"
    "${SIDX_SRC_DIR}/storagemanager/Buffer.h"
    "${SIDX_SRC_DIR}/storagemanager/DiskStorageManager.cc"
    "${SIDX_SRC_DIR}/storagemanager/DiskStorageManager.h"
    "${SIDX_SRC_DIR}/storagemanager/MemoryStorageManager.cc"
    "${SIDX_SRC_DIR}/storagemanager/MemoryStorageManager.h"
    "${SIDX_SRC_DIR}/storagemanager/RandomEvictionsBuffer.cc"
    "${SIDX_SRC_DIR}/storagemanager/RandomEvictionsBuffer.h"
    )
list(APPEND SIDX_CPP ${SIDX_STORAGEMANAGER_CPP})

set(SIDX_TOOLS_CPP
    "${SIDX_SRC_DIR}/tools/PointerPool.h"
    "${SIDX_SRC_DIR}/tools/PoolPointer.h"
    "${SIDX_SRC_DIR}/tools/rand48.cc"
    "${SIDX_SRC_DIR}/tools/rand48.cc"
    "${SIDX_SRC_DIR}/tools/rand48.h"
    "${SIDX_SRC_DIR}/tools/Tools.cc"
    "${SIDX_SRC_DIR}/tools/Tools.h"
    )
list(APPEND SIDX_CPP ${SIDX_TOOLS_CPP})

#
# Group source files for IDE source explorers (e.g. Visual Studio)
#

source_group("CMake Files" FILES CMakeLists.txt)

source_group("Source Files" FILES ${SIDX_BASE_CPP})


set(APPS_CPP_DEPENDENCIES "${SIDX_LIB_NAME}" CACHE INTERNAL "libraries to link")

###############################################################################
# Targets settings

set(SIDX_SOURCES
    ${SIDX_CPP})

# NOTE:
# This hack is required to correctly link static into shared library.
# Such practice is not recommended as not portable, instead each library,
# static and shared should be built from sources separately.
#if(UNIX)
#  add_definitions("-fPIC")
#endif()

if (WIN32)
  add_definitions("-DSIDX_DLL_EXPORT=1")
  if (NOT WITH_STATIC_LASZIP)
    add_definitions("-DSIDX_DLL_IMPORT=1")
  endif ()
endif ()

add_library(${SIDX_LIB_NAME} SHARED ${SIDX_SOURCES})

set_target_properties(${SIDX_LIB_NAME}
    PROPERTIES VERSION "${SIDX_LIB_VERSION}"
    SOVERSION "${SIDX_LIB_SOVERSION}")


if (APPLE)
  set_target_properties(
      ${SIDX_LIB_NAME}
      PROPERTIES
      INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" BUILD_WITH_INSTALL_RPATH OFF)
endif ()

###############################################################################
# Targets installation

install(TARGETS ${SIDX_LIB_NAME}
    RUNTIME DESTINATION ${SIDX_BIN_DIR}
    LIBRARY DESTINATION ${SIDX_LIB_DIR}
    ARCHIVE DESTINATION ${SIDX_LIB_DIR})

install(DIRECTORY ${SIDX_HEADERS_DIR}
    DESTINATION include/spatialindex
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
