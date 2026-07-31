cmake_minimum_required(VERSION 3.21)

project(music-radar
    VERSION 1.1
    DESCRIPTION "Music recognition app for the Linux desktop"
    HOMEPAGE_URL "https://github.com/keshavbhatt/music-radar"
    LANGUAGES CXX
)

set(APP_NAME "Music Radar")
set(APP_EXECUTABLE_NAME "music-radar")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/music-radar.desktop.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${APP_EXECUTABLE_NAME}.desktop"
    @ONLY
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Developed and shipped against Qt 6.11 (kde-qt6-core24-sdk / kf6-core24).
find_package(Qt6 6.4 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Multimedia
)

qt_standard_project_setup()

# Dev builds against the kde-qt6-core24-sdk snap on a non-Ubuntu host: some
# imported Qt targets export the SDK's generic /usr/include (Ubuntu glibc
# headers), which shadow the host toolchain's libc headers and break
# libstdc++. Strip that one directory from every imported target; the Qt
# headers proper live under .../include/<arch>/qt6 and are unaffected.
# Harmless in snapcraft builds (the dir then equals the real sysroot's).
if(CMAKE_PREFIX_PATH MATCHES "/snap/kde-qt6-core24-sdk/")
    get_property(_musicradar_imported DIRECTORY PROPERTY IMPORTED_TARGETS)
    foreach(_tgt IN LISTS _musicradar_imported)
        get_target_property(_incs ${_tgt} INTERFACE_INCLUDE_DIRECTORIES)
        if(_incs)
            set(_filtered "")
            foreach(_dir IN LISTS _incs)
                if(NOT _dir MATCHES "/snap/kde-qt6-core24-sdk/[^/]+/usr/include$")
                    list(APPEND _filtered "${_dir}")
                endif()
            endforeach()
            set_property(TARGET ${_tgt} PROPERTY
                INTERFACE_INCLUDE_DIRECTORIES "${_filtered}")
        endif()
    endforeach()
endif()

# Suppress qDebug output in non-debug builds.
add_compile_definitions(
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:QT_NO_DEBUG_OUTPUT>
)

# Plain add_executable (not qt_add_executable): the app has no QML, and
# skipping Qt target finalization avoids running the SDK snap's
# qmlimportscanner, which cannot start on non-Ubuntu dev hosts.
add_executable(${APP_EXECUTABLE_NAME}
    src/main.cpp
    src/about.cpp
    src/about.h
    src/about.ui
    src/elidedlabel.cpp
    src/elidedlabel.h
    src/home.cpp
    src/home.h
    src/home.ui
    src/iteminfowidget.cpp
    src/iteminfowidget.h
    src/iteminfowidget.ui
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/pulseutils.cpp
    src/pulseutils.h
    src/qaudiolevel.cpp
    src/qaudiolevel.h
    src/qaudiowaveform.cpp
    src/qaudiowaveform.h
    src/qaudiowaveform.ui
    src/rateapp.cpp
    src/rateapp.h
    src/rateapp.ui
    src/recordingpage.cpp
    src/recordingpage.h
    src/recordingpage.ui
    src/remotepixmaplabel2.cpp
    src/remotepixmaplabel2.h
    src/rungaurd.cpp
    src/rungaurd.h
    src/settingswidget.cpp
    src/settingswidget.h
    src/settingswidget.ui
    src/songitem.cpp
    src/songitem.h
    src/songitem.ui
    src/utils.cpp
    src/utils.h
    src/waitingspinnerwidget.cpp
    src/waitingspinnerwidget.h
    src/icons.qrc
    # Bundled Json widget module
    src/lib/Json/jsonhighlighter.cpp
    src/lib/Json/jsonhighlighter.h
    src/lib/Json/jsonitem.cpp
    src/lib/Json/jsonitem.h
    src/lib/Json/jsonmodel.cpp
    src/lib/Json/jsonmodel.h
    # QtStyler submodule (theme switcher)
    src/lib/QtStyler/qtstyler.cpp
    src/lib/QtStyler/qtstyler.h
    src/lib/QtStyler/stylesheets.qrc
)

# uic-generated headers include promoted-widget headers by bare name; the
# sources also include the bundled libs as <lib/...>.
target_include_directories(${APP_EXECUTABLE_NAME} PRIVATE src)

# The QtStyler submodule's header relies on QObject arriving transitively,
# which Qt 6 headers no longer provide. Force-include it instead of patching
# the (read-only) submodule; include guards make this free everywhere else.
target_compile_options(${APP_EXECUTABLE_NAME} PRIVATE -include QObject)

target_compile_definitions(${APP_EXECUTABLE_NAME} PRIVATE
    APPNAMESTR="${APP_NAME}"
    VERSIONSTR="${PROJECT_VERSION}"
)

target_link_libraries(${APP_EXECUTABLE_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Multimedia
)

# Install rules
include(GNUInstallDirs)
install(TARGETS ${APP_EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

if(NOT WIN32)
    install(FILES src/icons/music-radar.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${APP_EXECUTABLE_NAME}.desktop"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )
endif()
