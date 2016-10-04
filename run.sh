OMP_CANCELLATION=true
qmake QT_SELECT=qt4 -o Makefile build.pro
make -j $(nproc)


if [ -f MavenTests/test.xml ]; then
	rm test*.xml
fi

if [ -f bin/MavenTests ]; then
	./bin/MavenTests -xml
fi

