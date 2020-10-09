aux_source_directory(. SRC_HTTPCONN)

add_library(HttpConn ${SRC_HTTPCONN})

target_link_libraries(HttpConn Logger ConnectionPool Timer EventLoop Channel mysqlclient)