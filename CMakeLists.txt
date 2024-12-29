CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

# Set a default build type if none was specified
# ------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# About this project
# ------------------------------------------------------------------------------
project(partition_refinement)
SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")
SET(VERSION_PATCH "0")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Set environment
# ------------------------------------------------------------------------------
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

find_package(Git)
if(GIT_FOUND)
    message("git found: ${GIT_EXECUTABLE}")
else()
    message(WARNING "git not found. Cloning of submodules will not work.")
endif()


# Configure the compiler with the appropriate flags
# ------------------------------------------------------------------------------
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  # using Clang
  include(ConfigureCompilerClang)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  include(ConfigureCompilerGcc)
else ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	message(FATAL_ERROR "Only the compiler gcc and clang are supported")
endif()


# Targets
include_directories(${PROJECT_SOURCE_DIR})
include_directories(internal)
include_directories(external)
include_directories(lib)
include_directories(source)
# XML parser
include_directories(external/libxml2/install/include/libxml2)
link_directories(external/libxml2/install/lib)

add_executable(part-ref32.x source/sort.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_link_libraries(part-ref32.x PRIVATE xml2)

add_executable(part-ref32-verb.x source/sort.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(part-ref32-verb.x PUBLIC "-DVERBOSE")
target_link_libraries(part-ref32-verb.x PRIVATE xml2)

add_executable(part-ref64.x source/sort.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(part-ref64.x PUBLIC "-DM64")
target_link_libraries(part-ref64.x PRIVATE xml2)

add_executable(part-ref64-verb.x source/sort.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(part-ref64-verb.x PUBLIC "-DM64" "-DVERBOSE")
target_link_libraries(part-ref64-verb.x PRIVATE xml2)

add_executable(prune32.x source/prune.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_link_libraries(prune32.x PRIVATE xml2)

add_executable(prune32-verb.x source/prune.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(prune32-verb.x PUBLIC "-DVERBOSE")
target_link_libraries(prune32-verb.x PRIVATE xml2)

add_executable(prune64.x source/prune.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(prune64.x PUBLIC "-DM64")
target_link_libraries(prune64.x PRIVATE xml2)

add_executable(prune64-verb.x source/prune.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(prune64-verb.x PUBLIC "-DM64" "-DVERBOSE")
target_link_libraries(prune64-verb.x PRIVATE xml2)

add_executable(merge32.x source/merge.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_link_libraries(merge32.x PRIVATE xml2)

add_executable(merge32-verb.x source/merge.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(merge32-verb.x PUBLIC "-DVERBOSE")
target_link_libraries(merge32-verb.x PRIVATE xml2)

add_executable(merge64.x source/merge.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(merge64.x PUBLIC "-DM64")
target_link_libraries(merge64.x PRIVATE xml2)

add_executable(merge64-verb.x source/merge.cpp lib/part_ref.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(merge64-verb.x PUBLIC "-DM64" "-DVERBOSE")
target_link_libraries(merge64-verb.x PRIVATE xml2)

add_executable(index32.x source/index.cpp lib/part_ref.cpp lib/partial_order.cpp external/mpc/naive.cpp external/mpc/pflow.cpp external/mpc/graph.cpp external/tinyxml2/tinyxml2.cpp)
target_link_libraries(index32.x PRIVATE xml2)

add_executable(index32-verb.x source/index.cpp lib/part_ref.cpp lib/partial_order.cpp external/mpc/naive.cpp external/mpc/pflow.cpp external/mpc/graph.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(index32-verb.x PUBLIC "-DVERBOSE")
target_link_libraries(index32-verb.x PRIVATE xml2)

add_executable(index64.x source/index.cpp lib/part_ref.cpp lib/partial_order.cpp external/mpc/naive.cpp external/mpc/pflow.cpp external/mpc/graph.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(index64.x PUBLIC "-DM64")
target_link_libraries(index64.x PRIVATE xml2)

add_executable(index64-verb.x source/index.cpp lib/part_ref.cpp lib/partial_order.cpp external/mpc/naive.cpp external/mpc/pflow.cpp external/mpc/graph.cpp external/tinyxml2/tinyxml2.cpp)
target_compile_options(index64-verb.x PUBLIC "-DM64" "-DVERBOSE")
target_link_libraries(index64-verb.x PRIVATE xml2)