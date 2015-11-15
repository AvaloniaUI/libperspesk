#!/bin/bash
set -e
#set -x


function prepare
{
	ORIGDIR=`pwd`
	mkdir -p tmp/merge/tmp
	LIBDIR=`pwd`/$LIBDIR
	cd tmp/merge/tmp
	for lib in `echo $LIBDIR/*.a`
	do

		LIBNAME=`echo $lib|sed 's/.*\///' | sed 's/^lib//'|sed 's/.a$//'`
		echo Processing $LIBNAME
		for arch in `lipo -info $lib |sed 's/.*are..//'`
		do
			echo Processing $arch
			mkdir -p ../$arch
			rm -rf lib.a *.obj
			lipo -thin $arch $lib -output lib.a
			ar x lib.a
			for obj in *.o
			do
				strip -S $obj
				OUT="../$arch/$LIBNAME"_"$obj"
				mv $obj $OUT
			done
		done
	done
	cd $ORIGDIR
}

function prepare_all
{
	rm -rf tmp/merge
	LIBDIR=../../skia/xcodebuild/Release-iphonesimulator prepare
	LIBDIR=../../skia/xcodebuild/Release-iphoneos prepare
}

function pack_arch 
{
	cd $ARCH
	FILES="skia_core_* giflib_* png_* etc1_* jpeg-turbo_* SkKTX_* skia_codec_SkCodec.o skia_codec_SkCodec_libgif.o skia_codec_SkCodec_libpng.o skia_codec_SkJpeg*   skia_skgpu_* skia_ports_*  skia_utils_* skia_sfnt_*"
	FILES="$FILES skia_images_SkDecodingImageGenerator.o skia_images_SkImageDecoder.o skia_images_SkImageDecoder_FactoryRegistrar.o skia_images_SkImageDecoder_libbmp.o skia_images_SkImageDecoder_CG.o skia_images_SkImageDecoder_libjpeg.o skia_images_SkImageEncoder.o skia_images_SkImageEncoder_Factory.o skia_images_SkImageEncoder_argb.o skia_images_SkImageGenerator_skia.o skia_images_SkJpegUtility.o skia_images_SkMovie.o skia_images_SkPageFlipper.o skia_images_SkScaledBitmapSampler.o skia_images_bmpdecoderhelper.o "
	FILES="$FILES skia_effects_* skia_opts_SkBlitRow_opts_arm.o skia_opts_SkBlitMask_opts_arm.o skia_opts_SkBitmapProcState_opts_arm*"
	ar r ../$arch.a $FILES
	cd ..
}

prepare_all
