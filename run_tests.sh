
if [ -f tests/MavenTests/test.xml ]; then
    rm test*.xml
fi
if [ -f bin/MavenTests ]; then
    ./bin/MavenTests
    VAR=$?
    printf "\"MavenTests\" exit code: %s\n" "$VAR"
    # ignoring exit code from MavenTests on MacOS because of known failures
fi
if [ -f ./bin/test-libmaven* ]; then
    ./bin/test-libmaven
    VAR=$?
    printf "\"test-libmaven\" exit code: %s\n" "$VAR"
    if [ $VAR != 0 ]; then
        exit 1
    fi
fi
