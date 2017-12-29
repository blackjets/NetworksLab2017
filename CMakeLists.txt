cmake_minimum_required(VERSION 3.7)
project(zhkh)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES server.cpp server.h client.cpp client.h admin_client.cpp admin_client.h)
set(SOURCE_SOCKET_EXCEPTION_FILES socket_exception.cpp socket_exception.h) #было бы классно потом разобраться как это сделать нормально
set(SOURCE_SERVER_FILES server.cpp server.h meter.cpp meter.h socket_exception.cpp socket_exception.h epoll_exception.cpp epoll_exception.h)
set(SOURCE_CLIENT_FILES client.cpp client.h socket_exception.cpp socket_exception.h epoll_exception.cpp epoll_exception.h meter.h meter.cpp)
set(SOURCE_ADMCLIENT_FILES admin_client.cpp admin_client.h socket_exception.cpp socket_exception.h epoll_exception.cpp epoll_exception.h meter.h meter.cpp)
add_executable(Server ${SOURCE_SERVER_FILES})
add_executable(Client ${SOURCE_CLIENT_FILES})
add_executable(Admin_client ${SOURCE_ADMCLIENT_FILES})