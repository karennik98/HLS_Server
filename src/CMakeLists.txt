cmake_minimum_required(VERSION 3.1)

set(project_name hls_stream_server ) 
project(${project_name})

set(CMAKE_CXX_STANDARD 17)

add_library(${project_name}-lib
        AppComponent.hpp
        Utils.cpp
        Utils.hpp
        controller/MediaController.cpp
        controller/MediaController.hpp
        hls/Playlist.cpp
        hls/Playlist.hpp
        auth/AuthDataStorage.cpp
        auth/AuthDataStorage.hpp
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread")

target_include_directories(${project_name}-lib PUBLIC ${CMAKE_SOURCE_DIR}/src)

### link libs

find_package(oatpp REQUIRED)

### Attention: A LITTLE HACK TO MAKE OPENCV CMAKE WORK
set(OpenCV_DIR /usr/lib/cmake/opencv4)
###

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

message(STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS = ${OpenCV_LIBS}")
## linkage
target_link_libraries(${project_name}-lib PUBLIC oatpp::oatpp PUBLIC ${OpenCV_LIBS})

### where media is
add_definitions(
    -DEXAMPLE_MEDIA_FOLDER="${CMAKE_SOURCE_DIR}/video"
)

## add executables
add_executable(${project_name}
        App.cpp
)

target_link_libraries(${project_name} ${project_name}-lib ${OpenCV_LIBS})
add_dependencies(${project_name} ${project_name}-lib ${OpenCV_LIBS})

