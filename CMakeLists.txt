cmake_minimum_required(VERSION 2.8)
project(Netgen)
set(PACKAGE_VERSION "6.0-dev")
set(CMAKE_MFC_FLAG 0)

option( USE_OCC     "compile with OpenCascade geometry kernel" OFF)
option( USE_GUI     "don't build netgen with GUI" ON )
option( USE_MPI     "enable mpi parallelization" OFF )
option( USE_JPEG    "enable snapshots using library libjpeg" OFF )
option( USE_MPEG    "enable video recording with FFmpeg, uses libavcodec" OFF )
option( USE_PYTHON  "link boost python, necessary for ngsolve" ON )

if(INTEL_MIC)
    set(MKL_ARCH "mic")
    include(cmake_modules/mic.cmake)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmic")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmic")
else(INTEL_MIC)
    set(MKL_ARCH "intel64")
endif(INTEL_MIC)

# Append install paths of software in non-standard paths (e.g. openmpi, metis, intel mkl, ...)
# cmake -DUSE_MPI=ON -DADDITIONAL_PATHS="/opt/openmpi165;/opt/metis51" ../
set(ADDITIONAL_PATHS "" CACHE PATH "List of paths to additional libraries in non-standard locations, seperated by ';'")
if (ADDITIONAL_PATHS)
    list(REVERSE CMAKE_SYSTEM_PREFIX_PATH)
    list(APPEND CMAKE_SYSTEM_PREFIX_PATH ${ADDITIONAL_PATHS})
    list(REVERSE CMAKE_SYSTEM_PREFIX_PATH)
endif (ADDITIONAL_PATHS)

set(INSTALL_DIR "/opt/netgen" CACHE PATH "Prefix prepended to install directories")
set(CMAKE_INSTALL_PREFIX "${INSTALL_DIR}" CACHE INTERNAL "Prefix prepended to install directories" FORCE)
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -undefined dynamic_lookup")
#     set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

if(WIN32)
    set(ng_install_dir RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
else(WIN32)
    set(ng_install_dir RUNTIME DESTINATION bin LIBRARY DESTINATION lib)
endif(WIN32)
macro(get_WIN32_WINNT version)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REPLACE "." "" ver ${ver})
        string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})

        set(${version} "0x${ver}")
    endif()
endmacro()

macro(get_dll_from_lib dll_path lib_path)
        get_filename_component(parent_lib_path ${lib} DIRECTORY)
        get_filename_component(lib_name ${lib} name)
endmacro()

get_WIN32_WINNT(ver)

add_definitions(-DUSE_TIMEOFDAY)

if(WIN32)
# windows specific definitions and flags
    add_definitions(-D_WIN32_WINNT=${ver})
#     add_definitions(-DNGINTERFACE_EXPORTS)
#     add_definitions(-DNGLIB_EXPORTS)
    add_definitions(-DMSVC_EXPRESS -D_CRT_SECURE_NO_WARNINGS) 
# build convenience (aka object) libraries in windows)
    set(NG_LIB_TYPE OBJECT)
# omit openmp flags due to lacking support of openmp 3.0 in visual studio
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /wd4244 /wd4800") #${OpenMP_CXX_FLAGS} 
else(WIN32)
    set(NG_LIB_TYPE SHARED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11")
endif(WIN32)

include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories ("${PROJECT_SOURCE_DIR}/libsrc/include")
include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("/opt/X11/include")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake_modules")

find_package(OpenMP)
if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif(NOT WIN32)

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

if (USE_MPI)
    find_package(MPI REQUIRED)
    find_package(METIS REQUIRED)
    add_definitions(-DPARALLEL -DMETIS)
    include_directories(${MPI_CXX_INCLUDE_PATH})
    include_directories(${METIS_INCLUDE_DIR})
endif (USE_MPI)

if (USE_JPEG)
    find_package(JPEG REQUIRED)
    add_definitions(-DJPEGLIB)
    include_directories(${JPEG_INCLUDE_DIR})
endif (USE_JPEG)


if(USE_MKL)
    add_definitions(-DLINKMKL)
# TODO: find mkl
#set MKL_LIBRARIES
#     MKL_LIBS = -L$(MKLROOT)/lib/intel64  -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lmkl_def
endif(USE_MKL)


if (USE_GUI)
    set(CMAKE_THREAD_PREFER_PTHREAD ON)
    find_package(TclStub 8.5 REQUIRED)
    find_package(Threads REQUIRED)
    find_package(X11 REQUIRED)
    if(APPLE)
        set(OPENGL_glu_LIBRARY "/opt/X11/lib/libGLU.dylib") 
        set(OPENGL_gl_LIBRARY "/opt/X11/lib/libGL.dylib") 
    endif(APPLE)
    find_package(OpenGL REQUIRED)
    find_library(LIBPTHREAD NAMES pthread HINTS ${MY_LIB_DIR} )

    add_definitions(-DTCL -DOPENGL)
    include_directories(${TCL_INCLUDE_PATH})
#     include_directories(${TK_INCLUDE_PATH})

    if(WIN32)
        get_filename_component(MY_LIB_DIR ${TK_LIBRARY} DIRECTORY)
        find_library(LIBTOGL NAMES togl togl17 HINTS ${MY_LIB_DIR} )
#         message("MY_LIB_DIR:            ${MY_LIB_DIR}")
#         message("TK_LIBRARY :            ${TK_LIBRARY}")
        install( DIRECTORY "${MY_LIB_DIR}/tcl8.5"   DESTINATION lib )
        install( DIRECTORY "${MY_LIB_DIR}/tk8.5"    DESTINATION lib )
        install( DIRECTORY "${MY_LIB_DIR}/tix8.4.3" DESTINATION lib )
#TODO: install dlls needed by netgen.exe
        install( DIRECTORY "${MY_LIB_DIR}/../bin" DESTINATION bibinn)
    else(WIN32)
        set(LIBTOGL togl)
        ADD_DEFINITIONS(-DTOGL_X11)
    endif(WIN32)
endif (USE_GUI)

if (USE_PYTHON)
    set(Python_ADDITIONAL_VERSIONS 3.4)
    find_package(PythonInterp 3 REQUIRED)
    find_package(PythonLibs 3 REQUIRED)
    set(Boost_FIND_QUIETLY ON)
    if(WIN32)
# Visual studio needs python library without version number
        find_package(Boost COMPONENTS python)
    endif(WIN32)
    if(NOT Boost_LIBRARIES)
        find_package(Boost COMPONENTS python${PYTHON_VERSION_MAJOR})
    endif(NOT Boost_LIBRARIES)
    if(NOT Boost_LIBRARIES)
        find_package(Boost COMPONENTS python-py${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
    endif(NOT Boost_LIBRARIES)
    if(NOT Boost_LIBRARIES)
        message(FATAL_ERROR "Could NOT find Boost-Python" )
    endif(NOT Boost_LIBRARIES)
    add_definitions(-DNG_PYTHON)
    include_directories(${PYTHON_INCLUDE_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
    set(PYTHON_LIBS "${PYTHON_LIBRARIES};${Boost_LIBRARIES}")
    message("Boost-Python libs: ${PYTHON_LIBS}" )
    message("Boost-Python includes: ${PYTHON_INCLUDE_DIRS}" )
endif (USE_PYTHON)

include (CheckIncludeFiles)
check_include_files (dlfcn.h HAVE_DLFCN_H)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

add_definitions(-DHAVE_CONFIG_H)
add_subdirectory(libsrc)
add_subdirectory(ng)
add_subdirectory(tutorials)
add_subdirectory(python)
add_subdirectory(py_tutorials)
add_subdirectory(doc)
add_subdirectory(windows)
add_subdirectory(nglib)


set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES "/cmake/;/build/;/.gz/;~$;${CPACK_SOURCE_IGNORE_FILES}")
set(CPACK_PACKAGE_VERSION ${PACKAGE_VERSION} )

set(CPACK_PACKAGE_NAME netgen)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY Short description of netgen)
set(CPACK_PACKAGE_DESCRIPTION Long description of netgen)
if(UNIX)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "python3, libboost-python1.55.0, libtk8.5, libtcl8.5, tix, libxmu6")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64) # TODO should be set via: dpkg --print-architecture
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER Matthias Hochsteger <matthias.hochsteger@tuwien.ac.at>)
# TODO (use dpkg -s <packagename> to retrieve version)
    if(USE_MPI)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libmetis5, openmpi-bin")
        set(CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}_mpi")
    endif(USE_MPI)
# SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA  This variable allow advanced user to add custom script to the control.tar.gz (inside the .deb archive)  ${CMAKE_CURRENT_SOURCE_DIR}/postinst
    set(CPACK_DEBIAN_PACKAGE_SECTION Science)
    set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
endif(UNIX)
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
endif(WIN32)


include(CPack)

if(NOT WIN32)
    string(ASCII 27 Esc)
    set(ColorReset "${Esc}[m")
    set(BoldYellow  "${Esc}[1;34m")
endif(NOT WIN32)

function(message)
    _MESSAGE("${BoldYellow}${ARGV}${ColorReset}")
endfunction()

message("
------------------------------------------------------------------------
  ${PROJECT_NAME} ${PACKAGE_VERSION}:  Automatic configuration OK.

  Enabled functionality:

    OCC: ............... ${USE_OCC}
    JPEGlib: ........... ${USE_JPEG}
    FFMPEG: ............ ${USE_MPEG}
    GUI: ............... ${USE_GUI}
    MPI: ............... ${USE_MPI}
    PYTHON: ............ ${USE_PYTHON}


  Building: ")
if(WIN32)
message("
    Open ${CMAKE_BINARY_DIR}/Netgen.sln and build solution to compile ${PROJECT_NAME}.

    Build \"INSTALL\" to install ${PROJECT_NAME}.
")
else(WIN32)
message("
    Type 'make' to compile ${PROJECT_NAME}.

    Type 'make install' to install ${PROJECT_NAME}.
")
endif(WIN32)
message("
  Install directory:

    ${CMAKE_INSTALL_PREFIX}

  Example programs will be built but not installed.
------------------------------------------------------------------------
")








