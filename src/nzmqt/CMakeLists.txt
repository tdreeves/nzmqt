################################################################################

set(nzmqt_HEADERS
    nzmqt.hpp
)

set(nzmqt_SOURCES
    nzmqt.cpp
)

add_library (nzmqt ${nzmqt_HEADERS} ${nzmqt_SOURCES})
target_link_libraries (nzmqt cppzmq Qt5::Core)
target_include_directories(nzmqt PRIVATE ${PROJECT_BINARY_DIR})

generate_export_header(nzmqt)
################################################################################


install(
    TARGETS nzmqt
    ARCHIVE DESTINATION ${INSTALL_ARCHIVE_DIR}
    LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR}
    RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR}
)

install(FILES
    nzmqt.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/nzmqt_export.h DESTINATION ${INSTALL_INCLUDE_DIR}/nzmqt
)

################################################################################
