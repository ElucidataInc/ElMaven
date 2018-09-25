OMP_CANCELLATION=true
export QT_SELECT=qt5 
flag=100   #this flag check wether qt running in debug mode or not
while true; do
    read -p "Do you wish to install this program in release mode? " yn
    case $yn in
        [Yy]* ) qmake CONFIG+=release -o Makefile build.pro; break;;
        [Nn]* ) qmake CONFIG+=debug -o Makefile build.pro; flag=10; break;;
        * ) echo "Please answer yes or no.";;
    esac
done
make -j $(nproc)


if [ -f tests/MavenTests/test.xml ]; then
	rm test*.xml
fi

if [ -f ./bin/MavenTests ]; then
        echo "Running tests"
	./bin/MavenTests -xml
fi

