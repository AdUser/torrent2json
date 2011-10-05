add_executable(torrent2json "torrent2json.c")
add_executable(json2torrent "json2torrent.c")

target_link_libraries(torrent2json ${BUILD_LIBS})
target_link_libraries(json2torrent ${BUILD_LIBS})

install(TARGETS torrent2json json2torrent
        RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
