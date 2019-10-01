#include <fstream>
#include <sstream>

#include "Driver.h"
#include "Parser.h"
#include "Scanner.h"

namespace mgf
{

Driver::Driver(MgfFile& mgfFile)
        : trace_scanning(false),
        trace_parsing(false),
        context(mgfFile)
{}

bool Driver::parse_stream(std::istream& in, const std::string& sname) {
    streamname = sname;

    Scanner scanner(&in);
    scanner.set_debug(trace_scanning);
    this->lexer = &scanner;

    Parser parser(*this);
    parser.set_debug_level(trace_parsing);
    return (parser.parse() == 0);
}

bool Driver::parse_file(const std::string &filename) {
    std::ifstream in(filename.c_str());
    if (!in.good()) return false;
    return parse_stream(in, filename);
}

bool Driver::parse_string(const std::string &input, const std::string& sname) {
    std::istringstream iss(input);
    return parse_stream(iss, sname);
}

void Driver::error(const class location& l, const std::string& m) {
    std::cerr << l << ": " << m;
}

void Driver::error(const std::string& m) {
    std::cerr << m;
}

} // namespace mgf

