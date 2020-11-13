project(dependencies)
cmake_minimum_required(VERSION 3.17)

include(ExternalProject)

set(KF5_TAG v5.44.0)

set(ICOUTILS_VERSION 0.32.3)
set(ICOUTILS_SHA1 c8fb976eaceb63bffc3943e4e632f825b5abb5ac)

externalproject_add(extra-cmake-modules
    GIT_REPOSITORY https://invent.kde.org/frameworks/extra-cmake-modules.git
    GIT_TAG ${KF5_TAG}
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
)

externalproject_add(kguiaddons
    GIT_REPOSITORY https://invent.kde.org/frameworks/kguiaddons.git
    GIT_TAG ${KF5_TAG}
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    DEPENDS extra-cmake-modules
)

if (WIN32)
    # On Windows, ECM needs the icotool program from icoutils
    externalproject_add(icoutils
        URL https://downloads.sourceforge.net/project/unix-utils/icoutils/icoutils-${ICOUTILS_VERSION}-x86_64.zip
        URL_HASH SHA1=${ICOUTILS_SHA1}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND cp -R ../icoutils/bin/icotool.exe ../icoutils/bin/*.dll ${CMAKE_INSTALL_PREFIX}/bin
    )
endif()
