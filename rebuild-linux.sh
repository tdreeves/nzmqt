#! /bin/bash

set -e
set -x

# --------------------------------------------------------------------------------

build_debug=ON                  # OFF or ON

# --------------------------------------------------------------------------------

cd $(dirname $0)

# --------------------------------------------------------------------------------

function help()
{
  cat << EOF
  $(basename $0)
  -h | --help : show this help
  -d : build in debug mode to 'build' directory
  -r : build in release mode to 'rbuild' directory
EOF
}

while [ "$1" != "" ]; do
  case $1 in
    -h | --help )
      help
      exit 1
      ;;
    -d | --debug )
      build_debug=ON
      ;;
    -r | --release )
      build_debug=OFF
      ;;
    -* | --* )
      pkg_error "Bad option \"$1\""
      ;;
    * )
    pkg_files+=("$1")
  esac
  shift
done

# --------------------------------------------------------------------------------

abe_prefix=$HOME/abe/prefix
#qt5_prefix=$HOME/Qt5.12.2/5.12.2/gcc_64
qt5_prefix=$abe_prefix
qt5_lib_path=$qt5_prefix/lib

top_dir=$PWD
install_dir=$top_dir/build/prefix

if [ "$build_debug" = "ON" ]; then
  build_dir="build"
else
  build_dir="rbuild"
fi

# --------------------------------------------------------------------------------

# This is a QtCreator file. You may wish to remove it between builds.
#rm -f CMakeLists.txt.user

rm -rf $build_dir
mkdir $build_dir
cd $build_dir


#-G"CodeBlocks - Ninja" \
#-G"CodeBlocks - Unix Makefiles" \

cmake \
  -D ENABLE_TESTS=ON \
  -d BUILD_SHARED_LIBS=ON \
  \
  -G"CodeBlocks - Ninja" \
  -D CMAKE_BUILD_TYPE=$([ "$build_debug" = "ON" ] && echo "Debug" || echo "Release") \
  -D CMAKE_INSTALL_PREFIX=$install_dir \
  -D CMAKE_PREFIX_PATH="$install_dir;$abe_prefix;$qt5_prefix" \
  -D CMAKE_INSTALL_RPATH="$install_dir/lib;$abe_prefix/lib;$qt5_lib_path" \
  -D CMAKE_SKIP_BUILD_RPATH="FALSE" \
  -D CMAKE_BUILD_WITH_INSTALL_RPATH="FALSE" \
  -D CMAKE_INSTALL_RPATH_USE_LINK_PATH="TRUE" \
  ../

cmake --build .

cmake --build . --target install
