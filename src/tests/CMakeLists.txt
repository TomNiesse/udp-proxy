cmake_minimum_required(VERSION 3.16)

project(test_udptunnelconnection LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

add_executable(test_udptunnelpacketheader
    ../udptunnelpacketheader.cpp
    ../udptunnelpacketheader.h
    test_udptunnelpacketheader.cpp
    test_udptunnelpacketheader.h)

add_executable(test_udptunnelpacketmissingchunklist
    ../udptunnelpacketheader.cpp
    ../udptunnelpacketheader.h
    ../udptunnelpacketmissingchunklist.cpp
    ../udptunnelpacketmissingchunklist.h
    test_udptunnelpacketmissingchunklist.cpp
    test_udptunnelpacketmissingchunklist.h)

add_executable(test_udptunnelpacket
    ../udptunnelpacketheader.cpp
    ../udptunnelpacketheader.h
    ../udptunnelpacket.cpp
    ../udptunnelpacket.h
    ../udptunnelpacketmissingchunklist.cpp
    ../udptunnelpacketmissingchunklist.h
    test_udptunnelpacket.cpp
    test_udptunnelpacket.h)

add_executable(test_udptunnelconnectionsettings
    ../udptunnelconnectionsettings.cpp
    ../udptunnelconnectionsettings.h
    test_udptunnelconnectionsettings.cpp
    test_udptunnelconnectionsettings.h
    )

add_executable(test_udptunnelconnection
    ../udptunnelpacketmissingchunklist.cpp
    ../udptunnelpacketmissingchunklist.h
    ../udptunnelpacketbuffer.cpp
    ../udptunnelpacketbuffer.h
    ../udptunnelconnectionsettings.cpp
    ../udptunnelconnectionsettings.h
    ../udptunnelpacketheader.cpp
    ../udptunnelpacketheader.h
    ../udptunnelpacket.h
    ../udptunnelpacket.cpp
    ../iudptunnelpackethandler.h
    ../iudptunnelpackethandler.cpp
    ../udptunnelpacketsender.h
    ../udptunnelpacketsender.cpp
    ../udptunnelpacketreceiver.h
    ../udptunnelpacketreceiver.cpp
    ../udptunnelpacketchunkmanager.h
    ../udptunnelpacketchunkmanager.cpp
    ../udptunnelconnection.h
    ../udptunnelconnection.cpp
    test_udptunnelconnection.h
    test_udptunnelconnection.cpp)

add_test(NAME test_udptunnelconnectionsettings COMMAND test_udptunnelconnectionsettings)
add_test(NAME test_udptunnelconnection COMMAND test_udptunnelconnection)
add_test(NAME test_udptunnelpacketheader COMMAND test_udptunnelpacketheader)
add_test(NAME test_udptunnelpacketmissingchunklist COMMAND test_udptunnelpacketmissingchunklist)
add_test(NAME test_udptunnelpacket COMMAND test_udptunnelpacket)

target_link_libraries(test_udptunnelconnectionsettings PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(test_udptunnelconnection PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(test_udptunnelpacketheader PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(test_udptunnelpacketmissingchunklist PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(test_udptunnelpacket PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
