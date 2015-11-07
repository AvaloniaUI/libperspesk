#!/usr/bin/make -f
SKIA_DIR=../../skia
ANDROID_TOOLCHAINS=$(SKIA_DIR)/platform_tools/android//toolchains/
ANDROID_TOOLCHAIN=$(ANDROID_TOOLCHAINS)/$(TOOLCHAIN)
ANDROID_TOOLCHAIN_CC=$(ANDROID_TOOLCHAIN)/bin/$(CC)

host:
	CC=/usr/bin/c++ OUTDIR=host LIBDIR=$(SKIA_DIR)/out/Release PLATFORM_LIBS="-lfreetype -lGL -lGLU -lEGL -lX11 -lz -lfontconfig -lpthread" make build-generic

android:
	ACONF=arm_v7 TOOLCHAIN=arm-r10e-14 CC=arm-linux-androideabi-g++ make build-android-config

build-android-config:
	CC=$(ANDROID_TOOLCHAIN_CC) OUTDIR=$(ACONF) LIBDIR=$(SKIA_DIR)/out/config/android-$(ACONF)/Release PLATFORM_LIBS="-L$(ANDROID_TOOLCHAIN)/sysroot/usr/lib/ $(PLATFORM_LIBS) -llog -lEGL -lGLESv2 -lstdc++ -lz -lsupc++ -lexpat -lcpu_features -lgnustl_shared" make build-generic

build-generic:
	$(eval INCLUDES := $(shell echo ' ../include' $(SKIA_DIR)/include/*| sed 's/ / -I/g'))
	mkdir -p build/$(OUTDIR)
	NAME=$(OUTDIR) ./mergelibs.sh
	$(CC) $(INCLUDES) -Wl,--version-script=libperspesk.version -fPIC  -std=c++11 -fpermissive -g  -shared -Wl,-soname,libperspesk.so -o build/$(OUTDIR)/libperspesk.so ../src/*.cpp  -Ltmp/merge/$(OUTDIR)  -L$(LIBDIR)/obj/gyp  -L$(LIBDIR)/lib -rdynamic -lmskia -letc1 -lSkKTX -lgiflib -ljpeg-turbo $(SKIA_EXTRAS)  -Wl,-Bdynamic $(PLATFORM_LIBS) -ldl -Wl,--no-undefined #

all: clean
	make host
	make android

clean:
	rm -rf build tmp

#-Wl,--no-undefined
