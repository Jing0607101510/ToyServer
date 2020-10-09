aux_source_directory(. SRC)

add_library(ConnectionPool ${SRC})

target_link_libraries(ConnectionPool Logger mysqlclient )