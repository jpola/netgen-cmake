#ifdef WIN32
#ifdef NG_PYTHON
#include <boost/python.hpp>
#include "nglib.h"

DLL_HEADER void ExportNetgenMeshing();
DLL_HEADER void ExportMeshVis();
DLL_HEADER void ExportCSG();
DLL_HEADER void ExportCSGVis();
DLL_HEADER void ExportGeom2d();
BOOST_PYTHON_MODULE(nglib) {
    ExportCSG();
    ExportCSGVis();
    ExportNetgenMeshing();
    ExportMeshVis();
	ExportGeom2d();
}
#endif
#endif
