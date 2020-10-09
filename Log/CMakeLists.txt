aux_source_directory(. SRC_LOG)

add_library(Log ${SRC_LOG})

target_link_libraries(Log Utils pthread)