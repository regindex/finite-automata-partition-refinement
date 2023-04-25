# ##############################################################################
# Compiler configuration
# ##############################################################################

# Add the basic compiler options
add_compile_options("-std=c++17")


# Add the basic compiler options for debug version
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3")
# Add the basic compiler options for release version
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ansi -march=native -funroll-loops -O3 -DNDEBUG")