aux_source_directory(. SRC)

add_library(EventLoop ${SRC})

target_link_libraries(EventLoop Channel Poller Timer Logger Utils EventLoopThread)