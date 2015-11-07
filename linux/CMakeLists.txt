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
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/../../skia/out/Release/lib)

add_library(perspesk SHARED ${SOURCE_FILES})

#-Wl,--no-undefined

target_link_libraries(perspesk  -rdynamic -Wl,-whole-archive libskia_core.a libskia_effects.a
        libskia_images.a libskia_opts.a libskia_opts_sse41.a libskia_opts_ssse3.a libpng_static.a libskia_utils.a
        libskia_ports.a libskia_sfnt.a libskia_skgpu.a libetc1.a libSkKTX.a

        libgiflib.a libwebp_dec.a libwebp_utils.a libjpeg-turbo.a libwebp_dsp.a libwebp_enc.a libwebp_dsp_enc.a

         -Wl,-no-whole-archive
        -lfreetype  -lGL -lGLU -lEGL -lX11 -lz  -lfontconfig -lpthread)
