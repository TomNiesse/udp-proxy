cmake_minimum_required(VERSION 3.16)

project(udpproxy LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-Ofast -Wall -Wextra -Wconversion -Werror -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-Ofast -Wall -Wextra -Wconversion -Werror -Wpedantic")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

add_executable(udpproxy
  main.cpp
  udptunnelconnection.h udptunnelconnection.cpp
  udptunnelpacket.h udptunnelpacket.cpp
  udptunnelpacketheader.h udptunnelpacketheader.cpp
  tcptunnelpacketheader.h tcptunnelpacketheader.cpp
  tcptunnelpacket.h tcptunnelpacket.cpp
  proxyclient.h proxyclient.cpp
  proxyserver.h proxyserver.cpp
  proxyrequest.h proxyrequest.cpp
  tcpproxyconnectionmanager.h tcpproxyconnectionmanager.cpp
  udptunnelpacketchunkmananger.h udptunnelpacketchunkmananger.cpp
  limits.h
  app.h app.cpp
  tcpclient.h tcpclient.cpp
  udptunnelpacketsender.h udptunnelpacketsender.cpp
  udptunnelpacketreceiver.h udptunnelpacketreceiver.cpp
  version.h

)
target_link_libraries(udpproxy Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)

include(GNUInstallDirs)
install(TARGETS udpproxy
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
