cmake_minimum_required(VERSION 3.4.1)
set(lib_src_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi)
include_directories(src/main/cpp/include)

#My C
add_library( ffmpeg_jni-lib
             SHARED
             src/main/cpp/ffmpeg_jni.c )
add_library( ffmpeg_object-lib
             SHARED
             src/main/cpp/ffmpeg_object.c )

find_library( # Sets the name of the path variable.
              log-lib
              log )

#FFMPEG Library
add_library(avformat_lib SHARED IMPORTED)
set_target_properties(avformat_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavformat-57.so)
add_library(avutil_lib SHARED IMPORTED)
set_target_properties(avutil_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavutil-55.so)

target_link_libraries( ffmpeg_jni-lib
                       ffmpeg_object-lib)

target_link_libraries( ffmpeg_object-lib
                       avformat_lib
                       avutil_lib
                       ${log-lib} )