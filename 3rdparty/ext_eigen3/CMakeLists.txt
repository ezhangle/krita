SET(EXTPREFIX_eigen3 "${EXTPREFIX}" )
ExternalProject_Add( ext_eigen3
    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL http://files.kde.org/krita/build/dependencies/eigen3-3.2.6.tar.gz
    URL_MD5 9f93bfc14ba0816220c9674e99b8c418

    INSTALL_DIR ${EXTPREFIX_eigen3}
    PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/dart.diff
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_eigen3} -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE} ${GLOBAL_PROFILE}

    UPDATE_COMMAND ""
    ALWAYS 0
)
