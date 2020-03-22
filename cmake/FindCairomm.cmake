# - Try to find Cairomm 1.0
# Once done, this will define
#
#  Cairomm_FOUND - system has Cairomm
#  Cairomm_INCLUDE_DIRS - the Cairomm include directories
#  Cairomm_LIBRARIES - link these to use Cairomm

include(LibFindMacros)

# Dependencies
# libfind_package(Cairomm Cairo)
# libfind_package(Cairomm SigC++)
find_package(Cairo REQUIRED)
find_package(SigC++ REQUIRED)
find_package(freetype REQUIRED)
find_package(fontconfig REQUIRED)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Cairomm_PKGCONF cairomm-1.0)

# Main include dir
find_path(Cairomm_INCLUDE_DIR
  NAMES cairomm/cairomm.h
  PATHS ${Cairomm_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES cairomm-1.0
)

find_path(Cairommconfig_INCLUDE_DIR
  NAMES cairommconfig.h
  PATHS ${Cairomm_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES cairomm-1.0
)

libfind_library(Cairomm cairomm 1.0)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(Cairomm_PROCESS_INCLUDES Cairomm_INCLUDE_DIR Cairommconfig_INCLUDE_DIR SigC++_INCLUDE_DIRS CAIRO_INCLUDE_DIRS FREETYPE_INCLUDE_DIRS Fontconfig_INCLUDE_DIRS)
set(Cairomm_PROCESS_LIBS Cairomm_LIBRARY CAIRO_LIBRARIES SigC++_LIBRARIES)
libfind_process(Cairomm)
