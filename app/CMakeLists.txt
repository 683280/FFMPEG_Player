cmake_minimum_required(VERSION 3.4.1)
include_directories(src/main/cpp/include)

add_library( ffmpeg_jni-lib
             SHARED
             src/main/cpp/ffmpeg_jni.c )
add_library( ffmpeg_object-lib
             SHARED
             src/main/cpp/ffmpeg_object.c )

find_library( # Sets the name of the path variable.
              log-lib
              log )
target_link_libraries( ffmpeg_jni-lib
                       ffmpeg_object-lib
                       ${log-lib} )