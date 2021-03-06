#pragma once

#include <map>
#include <string>
#include "parser.hpp"

// declare the YY_DECL as our custom parser driver
#define YY_DECL yy::parser::symbol_type yylex(driver& drv)

YY_DECL;

class driver {
   public:
    driver();

    std::map<std::string, int> variables;

    int result;

    // to run the parser on a given file
    int parse(const std::string& f);

    // name of the file being parsed
    std::string file;

    // handling the scanner
    // NOTE: defined in tokens.l
    void scan_begin();
    void scan_end();

    // token location
    yy::location location;
};
