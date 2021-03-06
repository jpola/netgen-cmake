# dist_pkgdata_DATA = cube.surf

add_definitions(-DNGLIB_EXPORTS)
# ADD_LIBRARY(nglib SHARED nglib.cpp)
# TARGET_LINK_LIBRARIES( nglib interface geom2d csg stl occ mesh ${OCCLIBS} ${MPI_CXX_LIBRARIES} )
set(nglib_sources nglib.cpp)

if(WIN32)
    set(nglib_objects 
        $<TARGET_OBJECTS:interface>
        $<TARGET_OBJECTS:geom2d>
        $<TARGET_OBJECTS:csg>
        $<TARGET_OBJECTS:stl>
        $<TARGET_OBJECTS:occ>
        $<TARGET_OBJECTS:mesh>
        $<TARGET_OBJECTS:gen>
        $<TARGET_OBJECTS:la>
        $<TARGET_OBJECTS:gprim>

        $<TARGET_OBJECTS:visual>
        $<TARGET_OBJECTS:csgvis>
#         $<TARGET_OBJECTS:geom2dvis>
#         $<TARGET_OBJECTS:occvis>
#         $<TARGET_OBJECTS:stlvis>
       )
endif(WIN32)

add_library(nglib SHARED ${nglib_sources} ${nglib_objects})

target_link_libraries( nglib ${LIBPTHREAD} ${OCCLIBS} ${LIBTOGL} ${MPI_CXX_LIBRARIES} ${OPENGL_LIBRARIES} ${TK_LIBRARY} ${TCL_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${X11_Xmu_LIB} ${JPEG_LIBRARIES} ${MKL_LIBRARIES} ${ZLIB_LIBRARIES} )

if(USE_PYTHON)
    target_link_libraries(nglib ${PYTHON_LIBS})
endif(USE_PYTHON)

# ADD_EXECUTABLE(ng_vol ng_vol.cpp)
# TARGET_LINK_LIBRARIES( ng_vol nglib interface geom2d csg stl occ mesh ${OCCLIBS} ${MPI_CXX_LIBRARIES} )
# 
# ADD_EXECUTABLE(ng_stl ng_stl.cpp)
# TARGET_LINK_LIBRARIES( ng_stl nglib interface geom2d csg stl occ mesh ${OCCLIBS} ${MPI_CXX_LIBRARIES} )

install(FILES netgen.py DESTINATION bin )
# INSTALL(TARGETS ng_vol ng_stl ${ng_install_dir})
install(TARGETS nglib ${ng_install_dir})
