cmake_minimum_required(VERSION 3.1.0)

project(krokonf
	DESCRIPTION "CROOK-5 configuration editor"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(TRANSLATIONS_INSTALL_SUBDIR share/krokonf/translations)

add_subdirectory(translations)

# ---- Target: krokonf ---------------------------------------------------

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(emcrk REQUIRED)

add_executable(krokonf
	src/crookconf.cpp
	src/crookconf.h
	src/kernelselect.cpp
	src/kernelselect.h
	src/main.cpp
	src/mainwindow.cpp
	src/mainwindow.h
	src/kernelselect.ui
	src/mainwindow.ui
	resources.qrc
)

target_include_directories(krokonf PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(krokonf PUBLIC SYSTEM ${EMCRK_INCLUDE_DIRS})
target_compile_options(krokonf PUBLIC -Wall)
target_compile_definitions(krokonf PRIVATE TRANSLATIONS_DIR="${CMAKE_INSTALL_PREFIX}/${TRANSLATIONS_INSTALL_SUBDIR}")

target_link_libraries(krokonf
	Qt5::Widgets
	${EMCRK_LIBRARIES}
)

install(TARGETS krokonf
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# vim: tabstop=4
