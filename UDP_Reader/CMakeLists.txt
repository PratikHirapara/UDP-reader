cmake_minimum_required (VERSION 3.7)

set (UDP_Reader_SOURCES
    UDP_Reader.cpp

    #include just needed class to prevent need for more dependencies
    #../Readers/ChunkMetaDatabase/ChunkMetaDatabase.cpp
    #../Readers/ChunkMetaDatabase/ChunkMetaDatabase.h
)

add_executable(UDP_Reader ${UDP_Reader_SOURCES})
target_include_directories(UDP_Reader PRIVATE
    "../"
    )


target_link_libraries(UDP_Reader CONAN_PKG::opencv) 
target_link_libraries(UDP_Reader CONAN_PKG::boost)
#if(WIN32)
#  target_link_libraries(ChunkPlayer CONAN_PKG::boost  Ws2_32)
#else()
#  target_link_libraries(ChunkPlayer ${Boost_LIBRARIES} ${PCAP_LIBRARY} pthread)
#endif(WIN32)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
        DESTINATION bin
        FILES_MATCHING PATTERN "*.dll"
        PATTERN "CMakeFiles" EXCLUDE
        )
