add_definitions(-DNGLIB_EXPORTS)
add_library(geom2d ${NG_LIB_TYPE} genmesh2d.cpp geom2dmesh.cpp geometry2d.cpp python_geom2d.cpp )

if(NOT WIN32)
    target_link_libraries(geom2d mesh ${PYTHON_LIBS})
    target_link_libraries(geom2d ${PYTHON_LIBS})
    install( TARGETS geom2d DESTINATION lib )
endif(NOT WIN32)

if(USE_GUI)
    add_library(geom2dvis ${NG_LIB_TYPE} geom2dpkg.cpp vsgeom2d.cpp)
    if(NOT WIN32)
        target_link_libraries(geom2dvis geom2d)
        install( TARGETS geom2dvis DESTINATION lib )
    endif(NOT WIN32)
endif(USE_GUI)

