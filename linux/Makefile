#!/usr/bin/make -f
SKIA_DIR=../../skia
ANDROID_TOOLCHAINS=$(SKIA_DIR)/platform_tools/android//toolchains/
ANDROID_TOOLCHAIN=$(ANDROID_TOOLCHAINS)/$(TOOLCHAIN)
ANDROID_TOOLCHAIN_CC=$(ANDROID_TOOLCHAIN)/bin/$(CC)

host:
	CC=/usr/bin/c++ OUTDIR=host LIBDIR=$(SKIA_DIR)/out/Release SKIA_EXTRAS="-lskia_opts_ssse3 -lskia_opts_sse41" PLATFORM_LIBS="-lfreetype -lGL -lGLU -lEGL -lX11 -lz -lfontconfig -lpthread" make build-generic

android:
	ACONF=arm_v7 TOOLCHAIN=arm-r10e-14 CC=arm-linux-androideabi-g++ SKIA_EXTRAS="-lskia_opts_neon -lwebp_dsp_neon" make build-android-config

build-android-config:
	CC=$(ANDROID_TOOLCHAIN_CC) OUTDIR=$(ACONF) LIBDIR=$(SKIA_DIR)/out/config/android-$(ACONF)/Release PLATFORM_LIBS="-L$(ANDROID_TOOLCHAIN)/sysroot/usr/lib/ $(PLATFORM_LIBS) -llog -lEGL -lGLESv2 -lstdc++ -lz -lsupc++ -lgnustl_shared" SKIA_EXTRAS="-lfreetype_static -lexpat -lcpu_features $(SKIA_EXTRAS)" make build-generic

build-generic:
	$(eval INCLUDES := $(shell echo ' ../include' $(SKIA_DIR)/include/*| sed 's/ / -I/g'))
	mkdir -p build/$(OUTDIR)		
	$(CC) $(INCLUDES) -fPIC  -std=c++11 -fpermissive -g  -shared -Wl,-soname,libperspesk.so -o build/$(OUTDIR)/libperspesk.so ../src/*.cpp  -L$(LIBDIR)  -L$(LIBDIR)/obj/gyp  -L$(LIBDIR)/lib -rdynamic -Wl,-whole-archive -Wl,-Bstatic -lskia_core -lskia_effects -lskia_images -lskia_opts $(SKIA_EXTRAS) -lpng_static -lskia_utils -lskia_ports -lskia_sfnt -lskia_skgpu -letc1 -lSkKTX -lgiflib -lwebp_dec -lwebp_utils -ljpeg-turbo -lwebp_dsp -lwebp_enc -lwebp_dsp_enc -Wl,-Bdynamic -Wl,-no-whole-archive $(PLATFORM_LIBS) 

all:
	make host
	make android


#-Wl,--no-undefined
