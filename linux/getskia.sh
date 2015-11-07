source ./env
cd ../..
git clone 'https://chromium.googlesource.com/chromium/tools/depot_tools.git'
git clone 'https://skia.googlesource.com/skia'
set -e
cd skia
tools/install_dependencies.sh
echo Running sync-and-gyp, this will take a while
export GYP_DEFINES=skia_shared_lib=1
git checkout gyp/common.gypi gyp/common_conditions.gypi gyp/common_variables.gypi
patch -p1 < ../libperspesk/linux/skia.patch
bin/sync-and-gyp
echo Running ninja
ninja -C out/Release

set +e
for arch in `echo arm_v7 arm64 x86 mips`
do
BUILDTYPE=Release ./platform_tools/android/bin/android_ninja -d $arch
done
