#!/usr/bin/make -f
SKIA_DIR=../../skia
ANDROID_TOOLCHAINS=$(SKIA_DIR)/platform_tools/android//toolchains/
ANDROID_TOOLCHAIN=$(ANDROID_TOOLCHAINS)/$(TOOLCHAIN)
ANDROID_TOOLCHAIN_CC=$(ANDROID_TOOLCHAIN)/bin/$(CC)
HOST:=$(shell uname)

all: clean host android

host:
ifeq ($(HOST),Linux)
	echo Linux
	$(eval DEFINES := -DUSE_X11 $(DEFINES))
endif
	DEFINES=$(DEFINES) CC=/usr/bin/c++ OUTDIR=$(HOST)/$(shell uname -p) LIBDIR=$(SKIA_DIR)/out/Release PLATFORM_LIBS="-lfreetype -lGL -lGLU -lEGL -lX11 -lz -lfontconfig -lpthread" make build-generic

android:
	ACONF=x86 TOOLCHAIN=x86-r10e-14 CC=i686-linux-android-g++ make build-android-config
	ACONF=arm_v7 TOOLCHAIN=arm-r10e-14 CC=arm-linux-androideabi-g++ make build-android-config

build-android-config:
	CC=$(ANDROID_TOOLCHAIN_CC) OUTDIR=Android/$(ACONF) LIBDIR=$(SKIA_DIR)/out/config/android-$(ACONF)/Release PLATFORM_LIBS="-L$(ANDROID_TOOLCHAIN)/sysroot/usr/lib/ $(PLATFORM_LIBS) -llog -lEGL -lGLESv2 -lstdc++ -lz -lsupc++ -landroid -lexpat -lcpu_features" make build-generic

build-generic:
	$(eval INCLUDES := $(shell echo ' ../include' $(SKIA_DIR)/include/*| sed 's/ / -I/g'))
	mkdir -p build/$(OUTDIR)
	$(eval OUTFILE := build/$(OUTDIR)/libperspesk.so)
	rm -rf $(OUTFILE)
	NAME=$(OUTDIR) ./mergelibs.sh
	$(eval IMAGES := $(shell for d in libpng.o libjpeg.o libgif.o; do echo $(LIBDIR)/obj/src/images/images.SkImageDecoder_$$d; done;) )

	$(CC) $(INCLUDES) $(DEFINES) -Os -fno-exceptions -Wl,-s -Wl,--version-script=libperspesk.version -fPIC -Wl,-gc-sections -std=c++11 -fpermissive -g  -shared -Wl,-soname,libperspesk.so -o $(OUTFILE) ../src/*.cpp $(IMAGES) -Ltmp/merge/$(OUTDIR)  -L$(LIBDIR)/obj/gyp  -L$(LIBDIR)/lib -rdynamic -lmskia -letc1 -lSkKTX -lgiflib -ljpeg-turbo $(SKIA_EXTRAS)  -Wl,-Bdynamic  $(PLATFORM_LIBS) -ldl  -Wl,--no-undefined 

clean:
	rm -rf build tmp

.PHONY: host android build-generic all clean
