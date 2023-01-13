cmake_minimum_required(VERSION 3.14)

project(docJsonServer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS HttpServer)


add_executable(docJsonServer
  main.cpp
  signingcalculation.h signingcalculation.cpp
  signingmanager.h signingmanager.cpp
  link.h link.cpp
  inmemoryblockchain.h inmemoryblockchain.cpp
  iblockchain.h
  iauthors.h iauthors.cpp
  author.h author.cpp
  inmemoryauthors.h inmemoryauthors.cpp
  authorfacade.h authorfacade.cpp
)

target_link_libraries(docJsonServer Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(docJsonServer Qt${QT_VERSION_MAJOR}::HttpServer)

install(TARGETS docJsonServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
