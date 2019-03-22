cmake_minimum_required (VERSION 2.6)
project (nzmqt)

# Also hard coded in nzmqt-config-version.cmake
set(NZMQT_VERSION_MAJOR 3)
set(NZMQT_VERSION_MINOR 2)
set(NZMQT_VERSION_PATCH 1)
set(NZMQT_VERSION_STRING ${NZMQT_VERSION_MAJOR}.${NZMQT_VERSION_MINOR}.${NZMQT_VERSION_PATCH})

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

find_package(Qt5Core REQUIRED)
find_library(ZMQ_LIBRARY zmq)
find_path(ZMQ_INCLUDE_DIR zmq.hpp)

include_directories(${CMAKE_SOURCE_DIR}/include ${ZMQ_INCLUDE_DIR})
add_definitions(-DNZMQT_LIB -DNZMQT_SHAREDLIB)

QT5_WRAP_CPP(nzmqt_HEADERS_MOC
 include/nzmqt/nzmqt.hpp
)

################################################################################

add_library (nzmqt SHARED src/nzmqt/nzmqt.cpp ${nzmqt_HEADERS_MOC})
target_link_libraries (nzmqt Qt5::Core ${ZMQ_LIBRARY})
set_target_properties(nzmqt PROPERTIES VERSION ${NZMQT_VERSION_STRING})

################################################################################

include_directories(${CMAKE_SOURCE_DIR}/src)

set(nzmqt_test_SOURCES
 src/app/main.cpp
)

set(nzmqt_test_HEADERS
 include/nzmqt/nzmqt.hpp
 src/common/SampleBase.hpp
 src/pubsub/Subscriber.hpp
 src/pubsub/Publisher.hpp
 src/pushpull/Sink.hpp
 src/pushpull/Worker.hpp
 src/pushpull/Ventilator.hpp
 src/reqrep/Requester.hpp
 src/reqrep/Replier.hpp
 src/app/NzmqtApp.hpp
)

QT5_WRAP_CPP(nzmqt_test_HEADERS_MOC ${nzmqt_test_HEADERS})

add_executable (nzmqt-test ${nzmqt_test_SOURCES} ${nzmqt_test_HEADERS} ${nzmqt_test_HEADERS_MOC})
target_link_libraries (nzmqt-test nzmqt)

################################################################################

install(TARGETS nzmqt DESTINATION lib)
install (DIRECTORY include/nzmqt/ DESTINATION include/nzmqt)
install(FILES nzmqt-config.cmake nzmqt-config-version.cmake DESTINATION lib/cmake/nzmqt-${NZMQT_VERSION_STRING})

################################################################################
