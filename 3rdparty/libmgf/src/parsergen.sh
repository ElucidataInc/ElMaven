flex_major_version=$(flex --version 2>&1 | head -n1 | grep -o  "[0-9\.]" | head -n1)
flex_minor_version=$(flex --version 2>&1 | head -n1 | grep -o  "[0-9\.]" | head -n3 | tail -n1)
bison_major_version=$(bison --version 2>&1 | head -n1 | grep -o  "[0-9\.]" | head -n1)
bison_minor_version=$(bison --version 2>&1 | head -n1 | grep -o  "[0-9\.]" | head -n3 | tail -n1)
echo "Found flex: v$flex_major_version.$flex_minor_version"
echo "Found bison: v$bison_major_version.$bison_minor_version"
if [ $flex_major_version -eq 2 ] && [ $flex_minor_version -gt 4 ] && [ $bison_major_version -eq 2 ] && [ $bison_minor_version -gt 3 ]; then
    echo "Suitable bison and flex binaries were detected, regenerating scanner and parser…"

    # run flex to refresh scanner
    flex -o Scanner.cpp Scanner.l

    # regenerate parser from the existing yacc grammar
    bison -v -o Parser.cpp Parser.ypp

    # do some file extension adaptions
    cat Parser.cpp | sed -e 's/Parser\.hpp/Parser.h/g;s/stack\.hh/stack.h/g;s/location\.hh/location.h/g' > Parser.cpp.tmp
    mv Parser.cpp.tmp Parser.cpp
    cat Parser.hpp | sed -e 's/Parser\.hpp/Parser.h/g;s/location\.hh/location.h/g;s/stack\.hh/stack.h/g' > Parser.h
    rm Parser.hpp
    mv stack.hh stack.h
    cat location.hh | sed -e 's/location\.hh/location.h/g;s/position\.hh/position.h/g' > location.h
    rm location.hh
    cat position.hh | sed -e 's/position\.hh/position.h/g' > position.h
else
    echo "To generate updated sources for grammar file, flex v2.5 or newer and bison executable between v2.3b and v2.7 is required."
    echo "Skipping parser regeneration."
fi
