cmake_minimum_required(VERSION 3.5)
project(HostSvc)

if (WIN32)
    set(ASIO_INCLUDE "D:/asio-1.10.6/include" CACHE PATH "ASIO库的include目录")
    set(WS_INCLUDE "D:/websocketpp" CACHE PATH "")
    include_directories(${ASIO_INCLUDE})

    set(PROTOBUF_SRC_ROOT_FOLDER "D:/protobuf-3.0.0-beta-2-build" CACHE PATH "")
    set(PROTOBUF_LIBRARY_DEBUG "D:/protobuf-3.0.0-beta-2-build/Debug/libprotobufd.lib" CACHE FILEPATH "")
    set(PROTOBUF_PROTOC_EXECUTABLE "D:/protobuf-3.0.0-beta-2-build/Release/protoc.exe" CACHE FILEPATH "")

    set(SDL_LIBRARY "D:/SDL2-2.0.4/VisualC/x64/Release/SDL2main.lib;D:/SDL2-2.0.4/VisualC/x64/Release/SDL2.lib;" CACHE PATH "")
    set(SDL_INCLUDE_DIR "D:/SDL2-2.0.4/include" CACHE PATH "")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0602")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")


else(WIN32)
    set(SDL_LIBRARY "/usr/lib/x86_64-linux-gnu/libSDL2main.a;/usr/lib/x86_64-linux-gnu/libSDL2.so;/usr/lib/x86_64-linux-gnu/libSDL2.a" CACHE FILEPATH "")
    set(SDL_INCLUDE_DIR "/usr/include/SDL2/" CACHE PATH "")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lprotobuf -lz")
    set(WS_INCLUDE "/home/lishunan/websocketpp" CACHE PATH "")
endif(WIN32)
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)
include_directories(${WS_INCLUDE})
set(SOURCE_FILES Server.cpp HostSvcCommon.hpp Connection.hpp TestObject.h RPCService.hpp RealObjectBase.hpp ProxyObjectBase.hpp ClientManager.hpp RepeatWork.h ConnectionHandler.h)
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${SDL_INCLUDE_DIR})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS example.proto)

add_library(ProxyRemoteScreen SHARED ProxyRemoteScreen.cpp RemoteScreen.hpp ${PROTO_SRCS} ${PROTO_HDRS})
add_library(RealRemoteScreen SHARED RealRemoteScreen.cpp RemoteScreen.hpp ${PROTO_SRCS} ${PROTO_HDRS})
include(GenerateExportHeader)
GENERATE_EXPORT_HEADER(RealRemoteScreen
                        BASE_NAME RealRemoteScreen
                        EXPORT_MACRO_NAME RealRemoteScreen_EXPORT
                        EXPORT_FILE_NAME RealRemoteScreen_Export.h
                        STATIC_DEFINE RealRemoteScreen_BUILT_AS_STATIC
                        )
GENERATE_EXPORT_HEADER(ProxyRemoteScreen
                        BASE_NAME ProxyRemoteScreen
                        EXPORT_MACRO_NAME ProxyRemoteScreen_EXPORT
                        EXPORT_FILE_NAME ProxyRemoteScreen_Export.h
                        STATIC_DEFINE ProxyRemoteScreen_BUILT_AS_STATIC)
add_executable(Server RPCServer.hpp RealRemoteScreen.h Client.hpp ${SOURCE_FILES} ${PROTO_SRCS} ${PROTO_HDRS} TestObject.h RPCService.hpp RealObjectBase.hpp ProxyObjectBase.hpp ClientManager.hpp RepeatWork.h ConnectionHandler.h)
add_executable(Client Client.cpp HostSvcCommon.hpp ProxyRemoteScreen.h ${PROTO_SRCS} ${PROTO_HDRS} RPCController.hpp TestObject.h RPCService.hpp RealObjectBase.hpp ProxyObjectBase.hpp RPCClient.h)
if (WIN32)
    target_link_libraries(ProxyRemoteScreen ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${SDL_LIBRARY} ${SSL_EAY_RELEASE} ${LIB_EAY_RELEASE})
    target_link_libraries(RealRemoteScreen ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${SDL_LIBRARY} ${SSL_EAY_RELEASE} ${LIB_EAY_RELEASE})
    target_link_libraries(Client ProxyRemoteScreen ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${SDL_LIBRARY} ${SSL_EAY_RELEASE} ${LIB_EAY_RELEASE})
    target_link_libraries(Server  RealRemoteScreen ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${SDL_LIBRARY} ${SSL_EAY_RELEASE} ${LIB_EAY_RELEASE})
else (WIN32)
    target_link_libraries(ProxyRemoteScreen ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${SDL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY})
    target_link_libraries(RealRemoteScreen ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${SDL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY})
    target_link_libraries(Client  ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${SDL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY})
    target_link_libraries(Server  ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${SDL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY})
endif (WIN32)