OMP_CANCELLATION=true
export QT_SELECT=qt5 
while true; do
    read -p "Do you wish to install this program in release mode? " yn
    case $yn in
        [Yy]* ) qmake CONFIG+=release -o Makefile build.pro; break;;
        [Nn]* ) qmake CONFIG+=debug -o Makefile build.pro; break;;
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

var="$(uname -s)"
if [ "${var:0:1}" == "L" ]; then
    lcov --capture --directory ./ --output-file coverage.info
    genhtml coverage.info --output-directory coverage
fi

