# ##############################################################################
# Compiler configuration
# ##############################################################################

message("Building in ${CMAKE_BUILD_TYPE} mode")

# Add the basic compiler options
add_compile_options("-std=c++17")

# Add the basic compiler options for debug version
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3 -fsanitize=address -g") # Activate with "cmake -DCMAKE_BUILD_TYPE=DEBUG .." 
# Add the basic compiler options for release version
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ansi -march=native -funroll-loops -O3 -DNDEBUG") # Activate with "cmake -DCMAKE_BUILD_TYPE=RELEASE .." 
