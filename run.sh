OMP_CANCELLATION=true
export QT_SELECT=qt5 
flag=100   #this flag check wether qt running in debug mode or not
while true; do
    read -p "Do you wish to install this program in release mode? " yn
    case $yn in
        [Yy]* )
            qmake CONFIG+=release -o Makefile build.pro;
            break;;
        [Nn]* )
            qmake CONFIG+=debug -o Makefile build.pro; flag=10; break;;
        * ) echo "Please answer yes or no.";;
    esac
done

make -j $(getconf _NPROCESSORS_ONLN)

if [[ "$OSTYPE" == "darwin"* && flag -eq 100 ]]; then
	if [[ -z "${SENTRY_MACOSX_BIN}" ]]; then
		echo "Could not find Sentry library or binaries."
	else
		echo "Sentry library and binaries found."
		install_name_tool -change @rpath/libsentry_crashpad.dylib $SENTRY_MACOSX_BIN/libsentry_crashpad.dylib bin/El-MAVEN.app/Contents/MacOS/El-MAVEN;
		cp $SENTRY_MACOSX_BIN/crashpad_handler bin/El-MAVEN.app/Contents/MacOS/;
	fi
fi
