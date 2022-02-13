#!/bin/bash

ACTION=$1

case $ACTION in

	init)
		cmake -E make_directory "$BUILD_DIR"
		echo === Building EMCRK ===========================================
		git clone https://github.com/jakubfi/emcrk
		cmake -E make_directory emcrk/build
		cmake emcrk -G "Unix Makefiles" -B emcrk/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/mingw64
		cmake --build emcrk/build
		cmake --install emcrk/build
		echo === Installing QT5 ===========================================
		pacman -S mingw-w64-x86_64-qt5-static
	;;

	configure)
		cmake "$SRC_DIR" -G "Unix Makefiles" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"
	;;

	build)
		cmake --build "$BUILD_DIR"
	;;

	install)
		cmake --install "$BUILD_DIR"
	;;

	*)
		echo "Unknown action: $ACTION"
		exit 1
	;;

esac

# vim: tabstop=4
