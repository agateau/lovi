# Functions shared between the $OS-dependencies.sh files

AQTINSTALL_VERSION=0.9.0
AQTINSTALL_ARCHIVES="qtbase qtimageformats qtsvg qttranslations qttools"

QT_ARCH_WINDOWS=win64_msvc2017_64
QT_ARCH_MACOS=clang_64
QT_VERSION=5.12.8

ECM_VERSION=5.69.0

CMAKE_VERSION=3.17.\*

install_qt() {
    echo_title "Installing Qt"
    local qt_install_dir=$INSTALL_DIR/qt
    local aqt_args
    if is_windows ; then
        aqt_args="windows desktop $QT_ARCH_WINDOWS"
    fi
    if is_macos ; then
        aqt_args="mac desktop"
    fi
    $PIP_INSTALL_CMD aqtinstall==$AQTINSTALL_VERSION
    $PYTHON_CMD -m aqt install --outputdir $qt_install_dir $QT_VERSION $aqt_args --archives $AQTINSTALL_ARCHIVES

    # Add Qt bin dir to $PATH so that CMake find qmake and tests can find Qt dlls
    prepend_path $(find $qt_install_dir -type d -a -name bin)

    # Add Qt plugins dir to $QT_PLUGIN_PATH because the official Qt installer
    # patches QtCore dll so that it finds its plugins, but aqt does not.
    # Not being able to find plugins causes tests to not run on macOS and
    # Windows because they can't find the matching platform plugin.
    add_env_var QT_PLUGIN_PATH $(find $qt_install_dir -type d -a -name plugins)
}

install_cmake() {
    echo_title "Installing CMake"
    $PIP_INSTALL_CMD cmake==$CMAKE_VERSION
}

install_cmake_based_dependencies() {
    echo_title "Installing cmake-based dependencies"
    local build_dir=$WORK_DIR/cmake
    mkdir -p "$INSTALL_DIR/bin"
    prepend_path "$INSTALL_DIR/bin"
    cmake -B $build_dir -S $CI_DIR/lib/cmake \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
    cmake --build $build_dir --config $BUILD_TYPE --parallel $NPROC
}
