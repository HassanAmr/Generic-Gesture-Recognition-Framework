cmake_minimum_required(VERSION 2.8)
project( Recorder )
find_package( OpenCV )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( Recorder Recorder.cpp )
target_link_libraries( Recorder ${OpenCV_LIBS} )