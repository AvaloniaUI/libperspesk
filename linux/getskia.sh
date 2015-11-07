source ./env
cd ../..
git clone 'https://chromium.googlesource.com/chromium/tools/depot_tools.git'
git clone 'https://skia.googlesource.com/skia'
set -e
cd skia
tools/install_dependencies.sh
echo Running sync-and-gyp, this will take a while
export GYP_DEFINES=skia_shared_lib=1
bin/sync-and-gyp
echo Running ninja
ninja -C out/Release
