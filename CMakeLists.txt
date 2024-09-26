cmake_minimum_required (VERSION 3.7)

project (UDP_Reader)
set(PRODUCT_VERSION_MAJOR 1)
set(PRODUCT_VERSION_MINOR 2)

set(CMAKE_CXX_STANDARD 14)
option(SKIP_CONAN_IMPORT "Skip conan import if there are already imported files" OFF)
if (WIN32)
    macro(get_WIN32_WINNT version)
        if (CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            # Check for Windows 10, b/c we'll need to convert to hex 'A'.
            if ("${verMajor}" MATCHES "10")
                set(verMajor "A")
                string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            endif ("${verMajor}" MATCHES "10")
            # Remove all remaining '.' characters.
            string(REPLACE "." "" ver ${ver})
            # Prepend each digit with a zero.
            string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            set(${version} "0x${ver}")
        endif(CMAKE_SYSTEM_VERSION)
    endmacro(get_WIN32_WINNT)

    get_WIN32_WINNT(ver)
    message(STATUS "Setting Win NT version to ${ver}")
    add_definitions(-D_WIN32_WINNT=${ver})
    
    #Disable not relevant warnings:
    add_compile_options("/wd4251") # C4251: 'X': class 'Y' needs to have dll-interface to be used by clients of class 'Z'

endif(WIN32)

if (USE_CONAN)
    set (DEPENDENCIES
        boost/1.76.0 #incompatible to PCL 1.8.1
        eigen/3.3.9
        #libpcap/1.10.1
        poco/1.11.0
        opencv/4.5.2
        )
    

    set( LFT_PKG_OPTIONS
            *:shared=True
            boost:without_locale=True
            boost:without_log=True
            poco:enable_active_record=False
            poco:enable_apacheconnector=False
            poco:enable_cppparser=False
            poco:enable_crypto=False
            poco:enable_data=False
            poco:enable_data_mysql=False
            poco:enable_data_odbc=False
            poco:enable_data_postgresql=False
            poco:enable_data_sqlite=False
            poco:enable_encodings=False
            poco:enable_json=True
            poco:enable_jwt=False
            poco:enable_mongodb=False
            poco:enable_net=True
            poco:enable_netssl=False
            poco:enable_pagecompiler=False
            poco:enable_pagecompiler_file2page=False
            poco:enable_pdf=False
            poco:enable_pocodoc=False
            poco:enable_redis=False
            poco:enable_sevenzip=False
            poco:enable_util=True
            poco:enable_xml=True
            poco:enable_zip=False
            opencv:with_ade=False
            opencv:with_tiff=False
            opencv:with_openexr=False
            opencv:with_webp=False
            opencv:with_quirc=False
            opencv:with_ffmpeg=False
            opencv:dnn=False
            #use static linking for libs with a licence allowing this to prevent issues with wrong loaded DLL
            expat:shared=False
            poco:shared=False
        )

    set(CONAN_CMAKE_SILENT_OUTPUT ON)
    include(conan.cmake)
    message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

    if (WIN32)
        file( GLOB anyDll "${CMAKE_BINARY_DIR}/PCapExporter/*.dll" )
        if (SKIP_CONAN_IMPORT AND anyDll)
            message(STATUS "Skip DLL import as there are already DLLs")
        else()
        # TODO Adjust the whole IMPORT section to copy only needed dlls to the PCapExporter folder
            set (LFT_CONAN_IMPORTS
                IMPORTS "bin, *.dll -> ./PCapExporter @ excludes=bin/archdatadir/*"
                IMPORTS ", *.dll -> ./PCapExporter @ excludes=bin/*"
                IMPORTS "bin, boost_program_options.dll -> ./UDP_Reader" 
                IMPORTS "bin, boost_filesystem.dll -> ./UDP_Reader" 
            )

        endif()
    else()
        message(WARNING "TODO Handle imports for non Windows build")
    endif()

    conan_cmake_configure(
        REQUIRES ${DEPENDENCIES}
        GENERATORS cmake CMakeDeps #cmake_find_package
        ${LFT_CONAN_IMPORTS}
        OPTIONS
            ${LFT_PKG_OPTIONS}
        )

    SET (CONAN_BUILD missing CACHE STRING "Build setting for conan build. Set to 'missing' to build missing binary packages. Defaults to 'never'" )
    conan_cmake_autodetect(settings)
    message(STATUS "Conan settings = ${settings}")
    conan_cmake_install(PATH_OR_REFERENCE .
                        BUILD ${CONAN_BUILD}
                        REMOTE lft
                        SETTINGS ${settings}
                        )

    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    set (CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH} )
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")
    conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
    include_directories( ${PCAP_INCLUDE_DIR} )
else(USE_CONAN)
    find_package(Boost 1.6 COMPONENTS filesystem program_options REQUIRED)
    find_package(Eigen3 REQUIRED)

    
    include_directories( ${Boost_INCLUDE_DIR} )

    add_library(CONAN_PKG::boost INTERFACE IMPORTED)
    target_link_libraries(CONAN_PKG::boost INTERFACE ${Boost_LIBRARIES})

    add_library(CONAN_PKG::opencv INTERFACE IMPORTED)
    target_link_libraries(CONAN_PKG::opencv INTERFACE ${OpenCV_LIBS})

endif(USE_CONAN)

if(UNIX)
    find_package(Threads REQUIRED)
    set(EXTRA_PLATFORM_LIBS Threads::Threads)
endif()

add_definitions(-DLFT_PRODUCT_EXPORT)

#set (LOGGER_USE_POCO true)
#add_definitions(-DLOGGER_USE_POCO)

set (HAS_FULL_POCO true)
add_definitions(-DHAS_POCO)

if(MSVC)
  add_definitions(-DWINDOWS -D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)


add_subdirectory(UDP_Reader)
#target_link_libraries(UDP_Reader PRIVATE CONAN_PKG::opencv) 
#add_subdirectory(Readers/INS/Generic)
#add_subdirectory(Data)


install(
    TARGETS UDP_Reader
    RUNTIME DESTINATION bin
)





