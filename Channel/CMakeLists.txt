aux_source_directory(. SRC)

add_library(Channel ${SRC})

target_link_libraries(Channel HttpConn)