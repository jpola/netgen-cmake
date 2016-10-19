# noinst_HEADERS = meshdoc.hpp mvdraw.hpp vispar.hpp \
# visual.hpp vssolution.hpp

add_definitions(-DNGINTERFACE_EXPORTS)
install(FILES soldata.hpp DESTINATION include)

# AM_CPPFLAGS =  $(MPI_INCLUDES) -I$(top_srcdir)/libsrc/include  -DOPENGL -D$(TOGL_WINDOWINGSYSTEM) $(OCCFLAGS) $(TCL_INCLUDES)

if(USE_GUI)
    add_library(visual ${NG_LIB_TYPE}
            meshdoc.cpp mvdraw.cpp 
            vsfieldlines.cpp vsmesh.cpp vssolution.cpp importsolution.cpp visualpkg.cpp
            )
    if(NOT WIN32)
        target_link_libraries( visual ${PYTHON_LIBS} ${MPI_CXX_LIBRARIES} ${OPENGL_LIBRARIES} ${TCL_LIBRARY} )
        install( TARGETS visual DESTINATION lib )
    endif(NOT WIN32)
else(USE_GUI)
    add_library(visual ${NG_LIB_TYPE} visual_dummy.cpp)
    if(NOT WIN32)
        install( TARGETS visual DESTINATION lib )
    endif(NOT WIN32)
endif(USE_GUI)





