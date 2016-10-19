SET( PYTHON_PACKAGES_DIR python )
# EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} "-cfrom distutils import sysconfig;print(sysconfig.get_python_lib())" OUTPUT_VARIABLE PYTHON_PACKAGES_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

INSTALL(FILES
        __init__.py meshing.py csg.py
        DESTINATION ${PYTHON_PACKAGES_DIR}/netgen
       )

