set(nzmqt_test1_SOURCES
    app/main.cpp
)

set(nzmqt_test1_HEADERS
    common/SampleBase.hpp
    pubsub/Subscriber.hpp
    pubsub/Publisher.hpp
    pushpull/Sink.hpp
    pushpull/Worker.hpp
    pushpull/Ventilator.hpp
    reqrep/Requester.hpp
    reqrep/Replier.hpp
    app/NzmqtApp.hpp
    )

add_executable (nzmqt-test1 ${nzmqt_test1_SOURCES} ${nzmqt_test1_HEADERS})
target_link_libraries (nzmqt-test1 nzmqt)
target_include_directories(nzmqt-test1 PRIVATE ${CMAKE_SOURCE_DIR})
