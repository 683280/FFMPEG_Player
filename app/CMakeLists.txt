cmake_minimum_required(VERSION 3.4.1)
set(lib_src_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi)
include_directories(src/main/cpp/include)

#My C
#add_library( ffmpeg_jni-lib
#             SHARED
#             src/main/cpp/ffmpeg_jni.c )
#add_library( ffmpeg_object-lib
#             SHARED
#             src/main/cpp/ffmpeg_object.c )
#add_library( surface_native-lib
#             SHARED
#             src/main/cpp/surface_native.c )
#add_library( opensl_es-lib
#             SHARED
#             src/main/cpp/opensl_native.c )
#add_library( queue-lib
#             SHARED
#             src/main/cpp/queue.c )
#add_library( codec-lib
#             SHARED
#             src/main/cpp/codec.c )
#---------------------------C++------------------
add_library( ffmpeg_jni-lib
             SHARED
             src/main/cpp/ffmpeg_jni.cpp )
add_library( media_player-lib
             SHARED
             src/main/cpp/player/media_player.cpp )
add_library( video_object-lib
             SHARED
             src/main/cpp/player/video_object.cpp )

add_library( audio_object-lib
             SHARED
             src/main/cpp/player/audio_object.cpp )
add_library( queue-lib
             SHARED
             src/main/cpp/my_queue.c )
add_library( ffmpeg_object-lib
             SHARED
             src/main/cpp/player/ffmpeg_object.cpp )

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
add_library(avcodec_lib SHARED IMPORTED)
set_target_properties(avcodec_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavcodec-57.so)
add_library(avswscale_lib SHARED IMPORTED)
set_target_properties(avswscale_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libswscale-4.so)
add_library(avswresample_lib SHARED IMPORTED)
set_target_properties(avswresample_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libswresample-2.so)

target_link_libraries( ffmpeg_jni-lib
                       media_player-lib)

target_link_libraries( media_player-lib
                       video_object-lib
                       ffmpeg_object-lib
                       audio_object-lib)

target_link_libraries( video_object-lib
                       android
                       queue-lib
                       ${log-lib} )

target_link_libraries( audio_object-lib
                       OpenSLES
                       avswresample_lib
                       avutil_lib
                       queue-lib
                       android
                       ${log-lib} )

target_link_libraries( ffmpeg_object-lib
                       avutil_lib
                       avformat_lib
                       queue-lib
                       avcodec_lib
                       ${log-lib} )

#-----------------------------------------------------C------------
#target_link_libraries( ffmpeg_object-lib
#                       surface_native-lib
#                       codec-lib
#                       opensl_es-lib
#                       queue-lib
#                       avformat_lib
#                       avutil_lib
#                       avcodec_lib
#                       ${log-lib} )
#target_link_libraries( surface_native-lib
#                       android
#                       queue-lib
#                       avutil_lib
#                       avswscale_lib
#                       ${log-lib} )
#target_link_libraries( opensl_es-lib
#                       android
#                       avswresample_lib
#                       avutil_lib
#                       OpenSLES
#                       avswscale_lib
#                       ${log-lib} )
#target_link_libraries( queue-lib
#                       ${log-lib} )
#target_link_libraries( codec-lib
#                      avcodec_lib
#                      queue-lib
#                      ${log-lib} )
