#ifdef WIN32

// a wrapper to load netgen-dll into the executable

#include <mydefs.hpp>

DLL_HEADER int NG_main(int argc, char ** argv);

int main(int argc, char ** argv) 
{
  return NG_main(argc, argv);
}

#else

// Dummy for linux?
int main(int argc, char ** argv)
{
  return 0;
}

#endif

