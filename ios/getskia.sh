source ./env
cd ../..
git clone 'https://chromium.googlesource.com/chromium/tools/depot_tools.git'
git clone 'https://skia.googlesource.com/skia
set -e
cd skia
git checkout 844a0b4
echo Running sync-and-gyp, this will take a while
export GYP_DEFINES="skia_os='ios' skia_arch_type='arm' armv7=1 arm_neon=0"

echo Sync and GYP
bin/sync-and-gyp

cd out/gyp
xcodebuild -project SampleApp.xcodeproj -configuration Release -sdk iphonesimulator
set +e
xcodebuild -project SampleApp.xcodeproj -configuration Release

