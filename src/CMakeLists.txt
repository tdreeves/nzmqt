include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(nzmqt)

if(ENABLE_TESTS)
  add_subdirectory(test1)
endif()
