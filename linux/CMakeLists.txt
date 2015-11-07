cmake_minimum_required(VERSION 3.3)
project(libperspesk)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(../include)

FILE(GLOB SKIA_DIRS ../../skia/include/*)
include_directories(${SKIA_DIRS})

FILE(GLOB SOURCE_FILES ../src/*.cpp)

if(CMAKE_COMPILER_IS_GNUCC)
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
endif()
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/../../skia/out/Release/)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/../../skia/out/Release/obj/gyp)

add_library(perspesk SHARED ${SOURCE_FILES})
SET(MYLIB -Wl,-whole-archive -ljpeg-turbo -lpng_static -lskia_codec -lskia_effects -lskia_opts -lskia_opts_avx -lskia_opts_sse42 -lskia_pdf -lskia_sfnt -lskia_skgputest -lskia_utils -lskia_xml -lskia_animator -lskia_core -lskia_images -lskia_opts_avx2 -lskia_opts_sse41 -lskia_opts_ssse3 -lskia_ports -lskia_skgpu -lskia_svg -lskia_views -lskia_xps -Wl,--no-whole-archive)



target_link_libraries(perspesk ${MYLIB})

