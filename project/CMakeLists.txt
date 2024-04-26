cmake_minimum_required(VERSION 3.21.1)                                                                                                                                                          
project(RK)

add_library(eigen INTERFACE)
target_include_directories(eigen INTERFACE eigen)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(ASSIMP REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)

add_executable(RK 
    
    final_flight_path.cpp main.cpp
)

target_link_libraries(RK 
    ${OPENGL_LIBRARIES}
    glfw
    eigen
    #${ASSIMP_LIBRARIES}
)

