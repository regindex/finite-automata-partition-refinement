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

add_executable(part-ref32.x main.cpp lib/part_ref.cpp internal/Automaton.hpp internal/partition.hpp internal/definitions.hpp)
target_link_libraries(part-ref32.x dl)

add_executable(part-ref32-verb.x main.cpp lib/part_ref.cpp internal/Automaton.hpp internal/partition.hpp internal/definitions.hpp)
target_link_libraries(part-ref32-verb.x dl)
target_compile_options(part-ref32-verb.x PUBLIC "-DVERBOSE")

add_executable(part-ref64.x main.cpp lib/part_ref.cpp internal/Automaton.hpp internal/partition.hpp internal/definitions.hpp)
target_link_libraries(part-ref64.x dl)
target_compile_options(part-ref64.x PUBLIC "-DM64")

add_executable(part-ref64-verb.x main.cpp lib/part_ref.cpp internal/Automaton.hpp internal/partition.hpp internal/definitions.hpp)
target_link_libraries(part-ref64-verb.x dl)
target_compile_options(part-ref64-verb.x PUBLIC "-DM64" "-DVERBOSE")

# add_executable(check32.x check_output.cpp internal/Automaton.hpp internal/partition.hpp internal/definitions.hpp)
# target_link_libraries(check32.x dl)

# add_executable(check32-verb.x check_output.cpp internal/Automaton.hpp internal/partition.hpp internal/definitions.hpp)
# target_link_libraries(check32-verb.x dl)
# target_compile_options(check32-verb.x PUBLIC "-DVERBOSE")

add_executable(merge32.x merge.cpp lib/part_ref.cpp internal/Automaton.hpp internal/partition.hpp internal/definitions.hpp)
target_link_libraries(merge32.x dl)

add_executable(merge32-verb.x merge.cpp lib/part_ref.cpp internal/Automaton.hpp internal/partition.hpp internal/definitions.hpp)
target_link_libraries(merge32-verb.x dl)
target_compile_options(merge32-verb.x PUBLIC "-DVERBOSE")