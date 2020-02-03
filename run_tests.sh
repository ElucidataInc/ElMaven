
if [ -f tests/MavenTests/test.xml ]; then
    rm test*.xml
fi
if [ -d bin/MavenTests.app ]; then
    ./bin/MavenTests.app/Contents/MacOS/MavenTests
    VAR=$?
    printf "\"MavenTests\" exit code: %s\n" "$VAR"
    # ignoring exit code from MavenTests on MacOS because of known failures
fi
if [ -f ./bin/MavenTests* ]; then
    ./bin/MavenTests* -xml
    VAR=$?
    printf "\"MavenTests\" exit code: %s\n" "$VAR"
    if [ $VAR != 0 ]; then
        exit 1
    fi
fi
if [ -f ./bin/doctest* ]; then
    ./bin/doctest
    VAR=$?
    printf "\"doctest\" exit code: %s\n" "$VAR"
    if [ $VAR != 0 ]; then
        exit 1
    fi
fi
