cmake_minimum_required(VERSION 3.16)

project(ClassfieldPhoneNumber LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(classfieldnum main.cpp NumberPreviewOld.h NumberPreview.h classfieldraw.h)


include(GNUInstallDirs)
install(TARGETS classfieldnum
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
