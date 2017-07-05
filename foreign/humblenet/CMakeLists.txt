CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(humblenet)

if(NOT EMSCRIPTEN)
    add_subdirectory(webrtc libwebrtc)
endif()

CreateLibrary(${PROJECT_NAME}
DIRS
    .
INCLUDES
    PUBLIC
        .
    PRIVATE
        ${SDL2_INCLUDE_DIR}
        ../lib/libwebsockets/lib
		webrtc/core
		webrtc/Microstack
DEFINES
	$<$<BOOL:${WIN32}>:WINSOCK2 MICROSTACK_NO_STDAFX>		
	$<$<NOT:$<BOOL:${WIN32}>>:_POSIX>
LINK
	$<$<NOT:$<BOOL:${EMSCRIPTEN}>>:webrtc websockets>
	$<$<BOOL:${WIN32}>:Iphlpapi.lib>
)

SET(HUMBLENET_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

if(WIN32)
set(LIB_LIST)

find_package(OpenSSL REQUIRED)

include_directories("${OPENSSL_INCLUDE_DIR}")
list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})

# Link against dynamic linking functions.
# (Don't link directly to libdl since it is not needed on all platforms, it's now a part of libc).
list(APPEND LIB_LIST ${CMAKE_DL_LIBS})

target_link_libraries(${PROJECT_NAME} ${LIB_LIST})
endif()
