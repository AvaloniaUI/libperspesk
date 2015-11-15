#!/usr/bin/make -f

all: prepare_repack pack lib merge

update: pack lib merge

prepare_pack:
	./prepare-repack.sh

pack:
	./repack.sh

lib:
	xcodebuild -project libperspesk/libperspesk.xcodeproj -configuration Release -sdk iphonesimulator
	xcodebuild -project libperspesk/libperspesk.xcodeproj -configuration Release
	lipo -create libperspesk/build/Release-iphoneos/liblibperspesk.a libperspesk/build/Release-iphonesimulator/liblibperspesk.a -output build/libperspesk.a

merge:
	libtool -static build/libperspesk.a build/libskia.a -o build/libperspesk_standalone.a	

clean:
	rm -rf build tmp

.PHONY: host android build-generic all clean pack lib
