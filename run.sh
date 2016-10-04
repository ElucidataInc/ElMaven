OMP_CANCELLATION=true
while true; do
    read -p "Do you wish to install this program in release mode? " yn
    case $yn in
        [Yy]* ) qmake CONFIG+=release QT_SELECT=qt4 -o Makefile build.pro; break;;
        [Nn]* ) qmake CONFIG+=debug QT_SELECT=qt4 -o Makefile build.pro; break;;
        * ) echo "Please answer yes or no.";;
    esac
done
make -j $(nproc)


if [ -f MavenTests/test.xml ]; then
	rm test*.xml
fi

if [ -f bin/MavenTests ]; then
	./bin/MavenTests -xml
fi

