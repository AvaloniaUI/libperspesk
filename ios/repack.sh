#!/bin/bash
set -e
#set -x

function pack_arch
{
        cd $ARCH
        FILES="skia_core_* giflib_* png_* etc1_* jpeg-turbo_* SkKTX_* skia_codec_SkCodec.o skia_codec_SkCodec_libgif.o skia_codec_SkCodec_libpng.o skia_codec_SkJpeg*   skia_skgpu_* skia_ports_*  skia_utils_* skia_sfnt_*"
        FILES="$FILES skia_images_SkDecodingImageGenerator.o skia_images_SkImageDecoder.o skia_images_SkImageDecoder_FactoryRegistrar.o skia_images_SkImageDecoder_libbmp.o skia_images_SkImageDecoder_CG.o skia_images_SkImageDecoder_libjpeg.o skia_images_SkImageEncoder.o skia_images_SkImageEncoder_Factory.o skia_images_SkImageEncoder_argb.o skia_images_SkImageGenerator_skia.o skia_images_SkJpegUtility.o skia_images_SkMovie.o skia_images_SkPageFlipper.o skia_images_SkScaledBitmapSampler.o skia_images_bmpdecoderhelper.o "
        FILES="$FILES skia_effects_* skia_opts_SkBlitRow_opts_arm.o skia_opts_SkBlitMask_opts_arm.o skia_opts_SkBitmapProcState_opts_arm*"
        ar r ../$arch.a $FILES
        cd ..
}


cd tmp/merge
rm -rf tmp
rm -rf *.a
for arch in `ls`
do
	ARCH=$arch pack_arch
done

mkdir -p ../../build
rm -rf ../../build/libskia.a
lipo -create *.a -output ../../build/libskia.a

exit 0
rm -rf tmp/merge/$NAME
mkdir -p tmp/merge/$NAME

LIBDIR=`readlink -f $LIBDIR`

cd tmp/merge/$NAME
for i in `echo $LIBDIR/*.a`
do
	ar x $i
done

ar r libmskia.a *.o
rm *.o
