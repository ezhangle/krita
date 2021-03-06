project(krita)

message(STATUS "Using CMake version: ${CMAKE_VERSION}")

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

set(MIN_QT_VERSION 5.4.0)
set(MIN_FRAMEWORKS_VERSION 5.7.0)

if (POLICY CMP0002)
    cmake_policy(SET CMP0002 OLD)
endif()

if (POLICY CMP0017)
    cmake_policy(SET CMP0017 NEW)
endif ()

if (POLICY CMP0022)
    cmake_policy(SET CMP0022 OLD)
endif ()

if (POLICY CMP0026)
    cmake_policy(SET CMP0026 OLD)
endif()

if (POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

if (POLICY CMP0046)
    cmake_policy(SET CMP0046 OLD)
endif ()

if (POLICY CMP0059)
    cmake_policy(SET CMP0059 OLD)
endif()

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 OLD)
endif()

if (POLICY CMP0054)
    cmake_policy(SET CMP0054 OLD)
endif()

if (POLICY CMP0064)
    cmake_policy(SET CMP0064 OLD)
endif()


if (APPLE)
    set(APPLE_SUPPRESS_X11_WARNING TRUE)
    set(KDE_SKIP_RPATH_SETTINGS TRUE)
    set(CMAKE_MACOSX_RPATH 1)
    set(BUILD_WITH_INSTALL_RPATH 1)
    add_definitions(-Wno-deprecated-register)
endif()

# QT5TODO: remove KDE4_BUILD_TESTS once all kde4_add_unit_test have been converted
# transitional forward compatibility:
# BUILD_TESTING is cmake standard, KDE4_BUILD_TESTS not used by ECM/KF5, but only
# macros in cmake/transitional. Just, Macros from cmake/transitional,
# incl. kde4_add_unit_test, are only picked up if no macros from kdelibs4 are installed,
# because that transitional path is appended. Prepending instead might possibly unwantedly
# mask ECM/KF5 macros. Not tested.
# So have BUILD_TESTING define KDE4_BUILD_TESTS.
if (BUILD_TESTING)
    set(KDE4_BUILD_TESTS TRUE)
else()
    set(KDE4_BUILD_TESTS FALSE)
endif()

 ######################
#######################
## Constants defines ##
#######################
######################

# define common versions of Krita applications, used to generate kritaversion.h
# update these version for every release:
set(KRITA_VERSION_STRING "3.1 Alpha")
set(KRITA_STABLE_VERSION_MAJOR 3) # 3 for 3.x, 4 for 4.x, etc.
set(KRITA_STABLE_VERSION_MINOR 1) # 0 for 3.0, 1 for 3.1, etc.
set(KRITA_VERSION_RELEASE 89)     # 89 for Alpha, increase for next test releases, set 0 for first Stable, etc.
set(KRITA_ALPHA 1) # uncomment only for Alpha
#set(KRITA_BETA 1) # uncomment only for Beta
#set(KRITA_RC 1) # uncomment only for RC
set(KRITA_YEAR 2016) # update every year

if(NOT DEFINED KRITA_ALPHA AND NOT DEFINED KRITA_BETA AND NOT DEFINED KRITA_RC)
    set(KRITA_STABLE 1) # do not edit
endif()

message(STATUS "Krita version: ${KRITA_VERSION_STRING}")

# Define the generic version of the Krita libraries here
# This makes it easy to advance it when the next Krita release comes.
# 14 was the last GENERIC_KRITA_LIB_VERSION_MAJOR of the previous Krita series
# (2.x) so we're starting with 15 in 3.x series.
if(KRITA_STABLE_VERSION_MAJOR EQUAL 3)
    math(EXPR GENERIC_KRITA_LIB_VERSION_MAJOR "${KRITA_STABLE_VERSION_MINOR} + 15")
else()
    # let's make sure we won't forget to update the "15"
    message(FATAL_ERROR "Reminder: please update offset == 15 used to compute GENERIC_KRITA_LIB_VERSION_MAJOR to something bigger")
endif()
set(GENERIC_KRITA_LIB_VERSION "${GENERIC_KRITA_LIB_VERSION_MAJOR}.0.0")
set(GENERIC_KRITA_LIB_SOVERSION "${GENERIC_KRITA_LIB_VERSION_MAJOR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      "${CMAKE_SOURCE_DIR}/cmake/modules")
LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/kde_macro")

message("Module path:" ${CMAKE_MODULE_PATH})

# fetch git revision for the current build

set(KRITA_GIT_SHA1_STRING "")
set(KRITA_GIT_BRANCH_STRING "")

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
get_git_branch(GIT_BRANCH)

if(GIT_SHA1 AND GIT_BRANCH)
    string(SUBSTRING ${GIT_SHA1} 0 7 GIT_SHA1)
    set(KRITA_GIT_SHA1_STRING ${GIT_SHA1})
    set(KRITA_GIT_BRANCH_STRING ${GIT_BRANCH})
endif()

if(NOT DEFINED RELEASE_BUILD)
    # estimate mode by CMAKE_BUILD_TYPE content if not set on cmdline
    string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_TOLOWER)
    set(RELEASE_BUILD_TYPES "release" "relwithdebinfo" "minsizerel")
    list(FIND RELEASE_BUILD_TYPES "${CMAKE_BUILD_TYPE_TOLOWER}" INDEX)
    if (INDEX EQUAL -1)
        set(RELEASE_BUILD FALSE)
    else()
        set(RELEASE_BUILD TRUE)
    endif()
endif()
message(STATUS "Release build: ${RELEASE_BUILD}")

 ############
#############
## Options ##
#############
############

option(PACKAGERS_BUILD "Build support of multiple CPU architectures in one binary. Should be used by packagers only or Krita developers. Only switch off when you're an artist optimizing a build for your very own machine." ON)
if (WIN32)
    option(USE_BREAKPAD "Build the crash handler for Krita (only on windows)" OFF)
endif ()

option(HIDE_SAFE_ASSERTS "Don't show message box for \"safe\" asserts, just ignore them automatically and dump a message to the terminal." ON)
configure_file(config-hide-safe-asserts.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-hide-safe-asserts.h)

 #######################
########################
## Productset setting ##
########################
#######################

# For predefined productsets see the definitions in KritaProducts.cmake and
# in the files in the folder cmake/productsets.

# Finding out the products & features to build is done in 5 steps:
# 1. have the user define the products/features wanted, by giving a productset
# 2. estimate all additional required products/features
# 3. estimate which of the products/features can be build by external deps
# 4. find which products/features have been temporarily disabled due to problems
# 5. estimate which of the products/features can be build by internal deps

# get the special macros
include(CalligraProductSetMacros)
include(MacroJPEG)
include(GenerateTestExportHeader)


# get the definitions of products, features and product sets
include(KritaProducts.cmake)

set(PRODUCTSET_DEFAULT "ALL")
# temporary migration support
if (CREATIVEONLY)
    set(WARN_ABOUT_CREATIVEONLY TRUE)
    set(PRODUCTSET_DEFAULT "CREATIVE")
endif ()

if(NOT PRODUCTSET)
    set(PRODUCTSET ${PRODUCTSET_DEFAULT} CACHE STRING "Set of products/features to build" FORCE)
endif()

if (RELEASE_BUILD)
    set(CALLIGRA_SHOULD_BUILD_STAGING FALSE)
else ()
    set(CALLIGRA_SHOULD_BUILD_STAGING TRUE)
endif ()

# finally choose products/features to build
calligra_set_productset(${PRODUCTSET})

 ########################
#########################
## Look for KDE and Qt ##
#########################
########################

find_package(ECM 1.7.0 REQUIRED NOMODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
include(ECMOptionalAddSubdirectory)
include(ECMAddAppIcon)
include(ECMSetupVersion)
include(ECMMarkNonGuiExecutable)
include(ECMGenerateHeaders)
include(GenerateExportHeader)
include(ECMMarkAsTest)
include(ECMInstallIcons)

include(CMakePackageConfigHelpers)
include(WriteBasicConfigVersionFile)
include(CheckFunctionExists)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)
include(KDE4Macros)

# do not reorder to be alphabetical: this is the order in which the frameworks
# depend on each other.
find_package(KF5 ${MIN_FRAMEWORKS_VERSION} REQUIRED COMPONENTS
        Archive
        Config
        WidgetsAddons
        Completion
        CoreAddons
        GuiAddons
        I18n
        ItemModels
        ItemViews
        WindowSystem
)

find_package(Qt5 ${MIN_QT_VERSION}
        REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Xml
        Network
        PrintSupport
        Svg
        Test
        Concurrent
)


set(QT_QTTEST_LIBRARY Qt5::Test)


include (MacroLibrary)
include (MacroAdditionalCleanFiles)
include (MacroAddFileDependencies)

macro_ensure_out_of_source_build("Compiling Krita inside the source directory is not possible. Please refer to the build instruction https://community.kde.org/Krita#Build_Instructions")

# Note: OPTIONAL_COMPONENTS does not seem to be reliable
# (as of ECM 5.15.0, CMake 3.2)
if (NOT WIN32 AND NOT APPLE)

    find_package(Qt5 ${MIN_QT_VERSION} REQUIRED X11Extras)

    find_package(Qt5DBus ${MIN_QT_VERSION} QUIET)
    set(HAVE_DBUS ${Qt5DBus_FOUND})
    macro_log_feature(${Qt5DBus_FOUND} "dbus" "Qt DBUS integration" "http://www.qt.io/" FALSE "" "Optionally used to provide a dbus api on Linux")

    find_package(KF5KIO ${MIN_FRAMEWORKS_VERSION} QUIET)
    macro_bool_to_01(KF5KIO_FOUND HAVE_KIO)
    macro_log_feature(${KF5KIO_FOUND} "KIO" "KDE's KIO Framework" "http://api.kde.org/frameworks-api/frameworks5-apidocs/kio/html/index.html" FALSE "" "Optionally used for recent document handling")

    find_package(KF5Crash ${MIN_FRAMEWORKS_VERSION} QUIET) 
    macro_bool_to_01(KF5Crash_FOUND HAVE_KCRASH)
    macro_log_feature(${KF5Crash_FOUND} "kcrash" "KDE's Crash Handler" "http://api.kde.org/frameworks-api/frameworks5-apidocs/kcrash/html/index.html" FALSE "" "Optionally used to provide crash reporting on Linux")

    find_package(X11)
    if(X11_FOUND)
        find_package(Qt5 ${MIN_QT_VERSION} REQUIRED NO_MODULE COMPONENTS X11Extras)
        set(HAVE_X11 TRUE)
        add_definitions(-DHAVE_X11)
    else()
        set(HAVE_X11 FALSE)
    endif()

    find_package(XCB COMPONENTS XCB ATOM)
    if(XCB_FOUND)
        set(HAVE_XCB TRUE)
    else()
        set(HAVE_XCB FALSE)
    endif()
else()
    set(HAVE_DBUS FALSE)
    set(HAVE_X11 FALSE)
    set(HAVE_XCB FALSE)
endif()

add_definitions(
  -DTRANSLATION_DOMAIN=\"krita\"
  -DQT_USE_QSTRINGBUILDER
  -DQT_STRICT_ITERATORS
  -DQT_NO_SIGNALS_SLOTS_KEYWORDS
  -DQT_USE_FAST_OPERATOR_PLUS
  -DQT_USE_FAST_CONCATENATION
  -DQT_NO_URL_CAST_FROM_STRING
  -DQT_DISABLE_DEPRECATED_BEFORE=0
)

#
# The reason for this mode is that the Debug mode disable inlining
#
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_KRITADEVS "-O3 -g" CACHE STRING "" FORCE)
endif()

if(UNIX)
    set(CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES};m")
endif()

if(WIN32)
    if(MSVC)
        # C4522: 'class' : multiple assignment operators specified
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4522")
    endif()
endif()

# enable exceptions globally
kde_enable_exceptions()


# only with this definition will all the FOO_TEST_EXPORT macro do something
# TODO: check if this can be moved to only those places which make use of it,
# to reduce global compiler definitions that would trigger a recompile of
# everything on a change (like adding/removing tests to/from the build)
if(BUILD_TESTING)
    add_definitions(-DCOMPILING_TESTS)
endif()

set(KRITA_DEFAULT_TEST_DATA_DIR ${CMAKE_SOURCE_DIR}/sdk/tests/data/)
macro(macro_add_unittest_definitions)
  add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
  add_definitions(-DFILES_OUTPUT_DIR="${CMAKE_CURRENT_BINARY_DIR}")
  add_definitions(-DFILES_DEFAULT_DATA_DIR="${KRITA_DEFAULT_TEST_DATA_DIR}")
endmacro()




# overcome some platform incompatibilities
if(WIN32)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/winquirks)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DNOMINMAX)
    set(WIN32_PLATFORM_NET_LIBS ws2_32.lib netapi32.lib)
endif()

# set custom krita plugin installdir
set(KRITA_PLUGIN_INSTALL_DIR ${LIB_INSTALL_DIR}/kritaplugins)

 ###########################
############################
## Required dependencies  ##
############################
###########################

find_package(PNG REQUIRED)

if (APPLE)
    # this is not added correctly on OSX -- see http://forum.kde.org/viewtopic.php?f=139&t=101867&p=221242#p221242
    include_directories(SYSTEM ${PNG_INCLUDE_DIR})
endif()

add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost REQUIRED COMPONENTS system) # for pigment and stage

##
## Test for GNU Scientific Library
##
macro_optional_find_package(GSL)
macro_log_feature(GSL_FOUND "GSL" "GNU Scientific Library" "http://www.gnu.org/software/gsl" FALSE "1.7" "Required by Krita's Transform tool.")
macro_bool_to_01(GSL_FOUND HAVE_GSL)
configure_file(config-gsl.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-gsl.h )

 ###########################
############################
## Optional dependencies  ##
############################
###########################

##
## Check for OpenEXR
##
macro_optional_find_package(ZLIB)
macro_log_feature(ZLIB_FOUND "zlib" "Compression library" "http://www.zlib.net/" FALSE "" "Optionally used by the G'Mic and the PSD plugins")
macro_bool_to_01(ZLIB_FOUND HAVE_ZLIB)


macro_optional_find_package(OpenEXR)
macro_log_feature(OPENEXR_FOUND "OpenEXR" "High dynamic-range (HDR) image file format" "http://www.openexr.com" FALSE "" "Required by the Krita OpenEXR filter")
macro_bool_to_01(OPENEXR_FOUND HAVE_OPENEXR)
set(LINK_OPENEXR_LIB)
if(OPENEXR_FOUND)
    include_directories(SYSTEM ${OPENEXR_INCLUDE_DIR})
    set(LINK_OPENEXR_LIB ${OPENEXR_LIBRARIES})
    add_definitions(${OPENEXR_DEFINITIONS})
endif()

macro_optional_find_package(TIFF)
macro_log_feature(TIFF_FOUND "tiff" "TIFF Library and Utilities" "http://www.remotesensing.org/libtiff" FALSE "" "Required by the Krita TIFF filter")

macro_optional_find_package(JPEG)
macro_log_feature(JPEG_FOUND "jpeg" "Free library for JPEG image compression. Note: libjpeg8 is NOT supported." "http://www.libjpeg-turbo.org" FALSE "" "Required by the Krita JPEG filter")

macro_optional_find_package(OpenJPEG)
macro_log_feature(OPENJPEG_FOUND "openjpeg" "Free library for JPEG 2000 image compression" "http://www.openjpeg.org" FALSE "" "Required by the Krita JPEG 2000 filter")

set(LIBRAW_MIN_VERSION  "0.16")
macro_optional_find_package(LibRaw ${LIBRAW_MIN_VERSION})
macro_log_feature(LIBRAW_FOUND "LibRaw" "Library to decode RAW images" "http://www.libraw.org" FALSE "" "Required to build the raw import plugin")

macro_optional_find_package(FFTW3)
macro_log_feature(FFTW3_FOUND "FFTW3" "A fast, free C FFT library" "http://www.fftw.org/" FALSE "" "Required by the Krita for fast convolution operators and some G'Mic features")
macro_bool_to_01(FFTW3_FOUND HAVE_FFTW3)

macro_optional_find_package(OCIO)
macro_log_feature(OCIO_FOUND "OCIO" "The OpenColorIO Library" "http://www.opencolorio.org" FALSE "" "Required by the Krita LUT docker")
macro_bool_to_01(OCIO_FOUND HAVE_OCIO)

##
## Look for OpenGL
##
# TODO: see if there is a better check for QtGui being built with opengl support (and thus the QOpenGL* classes)
if(Qt5Gui_OPENGL_IMPLEMENTATION)
    message(STATUS "Found QtGui OpenGL support")
else()
    message(FATAL_ERROR  "Did NOT find QtGui OpenGL support. Check your Qt configuration. You cannot build Krita without Qt OpenGL support.")
endif()

##
## Test for eigen3
##
find_package(Eigen3 REQUIRED)
macro_log_feature(EIGEN3_FOUND "Eigen" "C++ template library for linear algebra" "http://eigen.tuxfamily.org" FALSE "3.0" "Required by Krita")

##
## Test for exiv2
##
set(EXIV2_MIN_VERSION "0.16")
find_package(Exiv2 REQUIRED)
macro_log_feature(EXIV2_FOUND "Exiv2" "Image metadata library and tools" "http://www.exiv2.org" FALSE "0.16" "Required by Krita")

##
## Test for lcms
##
find_package(LCMS2 REQUIRED)
macro_log_feature(LCMS2_FOUND "LittleCMS" "Color management engine" "http://www.littlecms.com" FALSE "2.4" "Will be used for color management and is necessary for Krita")
if(LCMS2_FOUND)
    if(NOT ${LCMS2_VERSION} VERSION_LESS 2040 )
        set(HAVE_LCMS24 TRUE)
    endif()
    set(HAVE_REQUIRED_LCMS_VERSION TRUE)
    set(HAVE_LCMS2 TRUE)
endif()

##
## Test for Vc
##
set(OLD_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} )
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )
set(HAVE_VC FALSE)
if( NOT MSVC)
    macro_optional_find_package(Vc 1.1.0)
    macro_log_feature(Vc_FOUND "Vc" "Portable, zero-overhead SIMD library for C++" "https://github.com/VcDevel/Vc" FALSE "" "Required by the Krita for vectorization")
    macro_bool_to_01(Vc_FOUND HAVE_VC)
    macro_bool_to_01(PACKAGERS_BUILD DO_PACKAGERS_BUILD)
endif()
configure_file(config-vc.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-vc.h )

if(HAVE_VC)
    message(STATUS "Vc found!")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                        "${CMAKE_SOURCE_DIR}/cmake/vc")

    include (VcMacros)

    if(Vc_COMPILER_IS_CLANG)
        set(ADDITIONAL_VC_FLAGS "-Wabi -ffp-contract=fast -fPIC")
    elseif (NOT MSVC)
        set(ADDITIONAL_VC_FLAGS "-Wabi -fabi-version=0 -ffp-contract=fast -fPIC")
    endif()

    #Handle Vc master
    if(Vc_COMPILER_IS_GCC OR Vc_COMPILER_IS_CLANG)
        AddCompilerFlag("-std=c++11" _ok)
        if(NOT _ok)
            AddCompilerFlag("-std=c++0x" _ok)
        endif()
    endif()

    macro(ko_compile_for_all_implementations_no_scalar _objs _src)
    if(PACKAGERS_BUILD)
        vc_compile_for_all_implementations(${_objs} ${_src} FLAGS ${ADDITIONAL_VC_FLAGS} ONLY SSE2 SSSE3 SSE4_1 AVX AVX2+FMA+BMI2)
    else()
        set(${_objs} ${_src})
    endif()
    endmacro()

    macro(ko_compile_for_all_implementations _objs _src)
    if(PACKAGERS_BUILD)
        vc_compile_for_all_implementations(${_objs} ${_src} FLAGS ${ADDITIONAL_VC_FLAGS} ONLY Scalar SSE2 SSSE3 SSE4_1 AVX AVX2+FMA+BMI2)
    else()
        set(${_objs} ${_src})
    endif()
    endmacro()

    if (NOT PACKAGERS_BUILD)
        # Optimize everything for the current architecture
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Vc_DEFINITIONS}")
    endif ()
endif()
set(CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH} )

##
## Test for Xinput
##
if(NOT WIN32 AND NOT APPLE)
  set(REQUIRED_Xinput_FOUND ${X11_Xinput_FOUND})
else()
  set(REQUIRED_Xinput_FOUND TRUE)
endif()

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS})

if(WIN32)
    set(LIB_INSTALL_DIR ${LIB_INSTALL_DIR}
                        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
                        LIBRARY ${INSTALL_TARGETS_DEFAULT_ARGS}
                        ARCHIVE ${INSTALL_TARGETS_DEFAULT_ARGS} )
endif()

##
## Test endianess
##
include (TestBigEndian)
test_big_endian(CMAKE_WORDS_BIGENDIAN)

##
## Test for qt-poppler
##
macro_optional_find_package(Poppler)
macro_log_feature( POPPLER_FOUND "Poppler-Qt5" "A PDF rendering library" "http://poppler.freedesktop.org" FALSE "" "Required by the Krita PDF filter.")

##
## Test for pthreads (for G'Mic)
##
macro_optional_find_package(Threads)
macro_log_feature(Threads_FOUND "PThreads" "A low-level threading library" "" FALSE "" "Optionally used by the G'Mic plugin")

##
## Test for OpenMP (for G'Mic)
##
macro_optional_find_package(OpenMP)
macro_log_feature(OPENMP_FOUND "OpenMP" "A low-level parallel execution library" "http://openmp.org/wp/" FALSE "" "Optionally used by the G'Mic plugin")

##
## Test for Curl (for G'Mic)
##
macro_optional_find_package(CURL)
macro_log_feature(CURL_FOUND "CURL" "A tool to fetch remote data" "http://curl.haxx.se/" FALSE "" "Optionally used by the G'Mic plugin")

 ############################
#############################
## Add Krita helper macros ##
#############################
############################

include(MacroKritaAddBenchmark)

 ####################
#####################
## Define includes ##
#####################
####################

# for config.h and <toplevel/foo.h> includes (if any?)
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR} 
                           ${CMAKE_CURRENT_BINARY_DIR} 
                           ${CMAKE_SOURCE_DIR}/interfaces 
)

include_directories(
   ${CMAKE_SOURCE_DIR}/libs/version
   ${CMAKE_BINARY_DIR}/libs/version
)

 ###################################################
####################################################
## Detect which products/features can be compiled ##
####################################################
###################################################

calligra_drop_product_on_bad_condition( APP_KRITA
    EIGEN3_FOUND "Eigen devel not found"
    EXIV2_FOUND "libexiv2 devel not found"
    HAVE_REQUIRED_LCMS_VERSION "lcms devel not found"
    Boost_SYSTEM_FOUND "boost-system devel not found"
    REQUIRED_Xinput_FOUND "Xinput devel not found "
)

#############################################
####  Backward compatibility BUILD_x=off ####
#############################################

# workaround: disable directly all products which might be activated by internal
# dependencies, but belong to scope of old flag
calligra_drop_products_on_old_flag(krita APP_KRITA)

#############################################
####      Temporarily broken products    ####
#############################################

# If a product does not build due to some temporary brokeness disable it here,
# by calling calligra_disable_product with the product id and the reason,
# e.g.:
# calligra_disable_product(APP_KEXI "isn't buildable at the moment")

#############################################
####     Calculate buildable products    ####
#############################################

calligra_drop_unbuildable_products()

 ###################
####################
## Subdirectories ##
####################
###################

if(SHOULD_BUILD_APP_KRITA)
  add_subdirectory(krita)
endif()

# non-app directories are moved here because they can depend on SHOULD_BUILD_{appname} variables set above
add_subdirectory(libs)
add_subdirectory(plugins)

add_subdirectory( benchmarks )

macro_display_feature_log()

calligra_product_deps_report("product_deps")
calligra_log_should_build()

configure_file(KoConfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/KoConfig.h )
configure_file(config_convolution.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config_convolution.h)
configure_file(config-ocio.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-ocio.h )

check_function_exists(powf HAVE_POWF)
configure_file(config-powf.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-powf.h)
