cmake_minimum_required(VERSION 3.16)

project(udpproxy LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_CXX_FLAGS "-Ofast -pipe -Wall -Wextra -Wconversion -Wpedantic")
# set(CMAKE_CXX_FLAGS_DEBUG "-Ofast -pipe -Wall -Wextra -Wconversion -Wpedantic")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

add_executable(udpproxy
  main.cpp
  app.h app.cpp
  tcpclientconnectionmanager.h tcpclientconnectionmanager.cpp
  tcphostconnectionmanager.h tcphostconnectionmanager.cpp
  clientconnection.h clientconnection.cpp
  udptunnelconnection.h udptunnelconnection.cpp
  udptunnelpacket.h udptunnelpacket.cpp
  udptunnelpacketchunkmanager.h udptunnelpacketchunkmanager.cpp
  udptunnelpacketheader.h udptunnelpacketheader.cpp
  tcptunnelpacket.h tcptunnelpacket.cpp
  tcptunnelpacketheader.h tcptunnelpacketheader.cpp
  udptunnelconnectionsettings.h udptunnelconnectionsettings.cpp
  hostconnection.h hostconnection.cpp
  udptunnelconnectionportmanager.h udptunnelconnectionportmanager.cpp
  proxyserver.h proxyserver.cpp
  proxyrequest.h proxyrequest.cpp
  version.h
  udptunnelpacketreceiver.h udptunnelpacketreceiver.cpp
  udptunnelpacketmissingchunklist.h udptunnelpacketmissingchunklist.cpp
  iudptunnelpackethandler.h iudptunnelpackethandler.cpp
  udptunnelpacketsender.h udptunnelpacketsender.cpp
  tcptunnelclientconnection.h tcptunnelclientconnection.cpp
  tcptunnelhostconnection.h tcptunnelhostconnection.cpp
  udptunnelconnectionmanagerhost.h udptunnelconnectionmanagerhost.cpp
  proxytunnelhost.h proxytunnelhost.cpp
  proxytunnelclient.h proxytunnelclient.cpp
  udptunnelpacketbuffer.h udptunnelpacketbuffer.cpp
  tcptunnelconnectionsettings.h tcptunnelconnectionsettings.cpp
)
target_link_libraries(udpproxy Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)

include(GNUInstallDirs)
install(TARGETS udpproxy
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
add_subdirectory(tests)