project(dependencies)
cmake_minimum_required(VERSION 3.17)

include(ExternalProject)

set(KF5_TAG v5.44.0)

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
