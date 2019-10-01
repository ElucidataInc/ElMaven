# run flex to refresh scanner
flex -o Scanner.cpp Scanner.l

# generate parser from the existing yacc grammar
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
