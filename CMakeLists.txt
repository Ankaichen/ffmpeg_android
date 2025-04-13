cmake_minimum_required(VERSION 3.30)
project(ffmpeg_opengl)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/ffmpeg-7.0.2/include
)

link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/ffmpeg-7.0.2/lib
)

file(GLOB BIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/ffmpeg-7.0.2/bin/*)
file(COPY ${BIN_FILE} DESTINATION ${CMAKE_BINARY_DIR})

aux_source_directory(./ MAIN_SRCS) # 找到所有的源文件 不递归

add_executable(ffmpeg_opengl ${MAIN_SRCS})

target_link_libraries(ffmpeg_opengl avcodec)


