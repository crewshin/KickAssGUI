# - Try to find LibXml2
#  Once done this will define
#  LIBXML2_FOUND - System has LibXml2
#  LIBXML2_INCLUDE_DIRS - The LibXml2 include directories
#  LIBXML2_LIBRARIES - The libraries needed to use LibXml2
#  LIBXML2_DEFINITIONS - Compiler switches required for using LibXml2

#find_package(PkgConfig)
#pkg_check_modules(PC_LIBXML QUIET libxml-2.0)
#set(LIBXML2_DEFINITIONS ${PC_LIBXML_CFLAGS_OTHER})

find_path(Arnold_INCLUDE_DIR NAMES ai.h ai_api.h
                HINTS 	/Users/crewshin/Dropbox/_3D/Arnold/Arnold_osx/include
                		${Arnold_INCLUDE_DIR}
                		C:\\DEV\\Arnold\\Arnold_win\\include\\ )

find_library(Arnold_LIBRARY NAMES ai libai libOpenImageIO
                HINTS	/Users/crewshin/Dropbox/_3D/Arnold/Arnold_osx/bin
                		${Arnold_LIBRARY}
                		C:\\DEV\\Arnold\\Arnold_win\\lib\\ )

set(Arnold_LIBRARIES ${Arnold_LIBRARY})
set(Arnold_INCLUDE_DIRS ${Arnold_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Arnold DEFAULT_MSG
                                  Arnold_LIBRARY Arnold_INCLUDE_DIR)

mark_as_advanced(Arnold_INCLUDE_DIR Arnold_LIBRARY)
