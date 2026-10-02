cmake_minimum_required(VERSION 3.25)

# project(deluxe_lib
#     VERSION 0.1.0
#     LANGUAGES CXX
# )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(core)
add_subdirectory(math)
add_subdirectory(network)
