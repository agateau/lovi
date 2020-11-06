mingw_prefix=mingw-w64-$(uname -m)

pacman -S --needed --noconfirm \
    make \
    $mingw_prefix-gcc \
    $mingw_prefix-binutils \
    $mingw_prefix-cmake \
    $mingw_prefix-qt5 \
    $mingw_prefix-kguiaddons-qt5 \
    $mingw_prefix-extra-cmake-modules \
    $mingw_prefix-python-pip \
    $mingw_prefix-icoutils \
    $mingw_prefix-ninja

install_qpropgen_dependencies
