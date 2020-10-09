aux_source_directory(. SRC)

add_library(EventLoopThreadPool ${SRC})

target_link_libraries(EventLoopThreadPool EventLoop Utils Logger)