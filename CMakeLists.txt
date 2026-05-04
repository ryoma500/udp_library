cmake_minimum_required(VERSION 3.10)

project(udp_library)

# ライブラリ作成（静的ライブラリ）
add_library(udp_library STATIC
    src/udp_library.cpp
)

# ヘッダ公開ディレクトリ
target_include_directories(udp_library PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Windows用ソケットライブラリ
if(WIN32)
    target_link_libraries(udp_library PUBLIC ws2_32)
endif()