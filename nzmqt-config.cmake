# Sets the following variables:
# NZMQT_FOUND
# NZMQT_LIBRARIES
# NZMQT_DEFINITIONS
# NZMQT_INCLUDES

find_library(ZMQ_LIBRARY zmq)
find_path(NZMQT_INCLUDE_DIR nzmqt/nzmqt.hpp)
find_library(NZMQT_LIBRARY nzmqt)

# handle the QUIETLY and REQUIRED arguments and set NZMQT_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NZMQT DEFAULT_MSG ZMQ_LIBRARY NZMQT_INCLUDE_DIR NZMQT_LIBRARY)

if (NZMQT_FOUND)
	message ("NZMQT_FOUND")
	set (NZMQT_DEFINITIONS -DNZMQT_LIB -DNZMQT_SHAREDLIB)
	set (NZMQT_INCLUDES ${NZMQT_INCLUDE_DIR})
	set (NZMQT_LIBRARIES ${ZMQ_LIBRARY} ${NZMQT_LIBRARY})
endif (NZMQT_FOUND)

unset(ZMQ_LIBRARY)
unset(NZMQT_INCLUDE_DIR)
unset(NZMQT_LIBRARY)

mark_as_advanced(nzmqt_FOUND NZMQT_DEFINITIONS NZMQT_LIBRARIES NZMQT_INCLUDES)
