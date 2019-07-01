
if [ -f tests/MavenTests/test.xml ]; then
	rm test*.xml
fi

if [ -d bin/MavenTests.app ]; then
	./bin/MavenTests.app/Contents/MacOS/MavenTests
	VAR=$?
	echo $VAR
	if [ $VAR != 0 ]; then
		exit 1
	fi
fi

if [ -f ./bin/MavenTests* ]; then
	./bin/MavenTests* -xml
    VAR=$?
    echo $VAR
    if [ $VAR != 0 ]; then
        exit 1
    fi
fi

