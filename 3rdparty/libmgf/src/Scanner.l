/* $Id$ */

/*
 * C/C++ Declarations 
 */
%{
#include "Scanner.h"

#include <string>
#include <sstream>
#include <iostream>

/* import the parser's token type into a local typedef */
typedef mgf::Parser::token token;
typedef mgf::Parser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate by
 * default returns 0, which is not of token_type. */
#define yyterminate() return token::END

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H
%}

/*
 * Flex Declarations and Options 
 */

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "ExampleFlexLexer" */
%option prefix="Mgf"

/* the manual says "somewhat more optimized" */
%option batch

/* enable scanner to generate debug output. disable this for release
 * versions. */
/* %option debug */

/* no support for include files is planned */
%option yywrap nounput 

/* enables the use of start condition stacks */
%option stack

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%% /*** Regular Expressions Part ***/ 

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}

 /*
  * lexer rules 
  */
[0-9]+("."[0-9]+)?[eE][+-][0-9]+ {
    std::istringstream iss;
    iss.str(std::string(yytext));
    iss >> yylval->doubleVal;
    return token::DOUBLE;
}

[0-9]+"."[0-9]* {
    yylval->doubleVal = atof(yytext);
    return token::DOUBLE;
}

[0-9]+ {
    yylval->integerVal = atoi(yytext);
    return token::INTEGER;
}

"BEGIN IONS"[ \r\t]* {
    yylloc->step();
    return token::BEGIN_IONS;
}

"CLE" {
    yylloc->step();
    return token::CLE;
}
"DB" {
    yylloc->step();
    return token::DB;
}
"FORMAT" {
    yylloc->step();
    return token::FORMAT;
}
"ITOLU" {
    yylloc->step();
    return token::ITOLU;
}
"MASS" {
    yylloc->step();
    return token::MASS;
}
"MODS" {
    yylloc->step();
    return token::MODS;
}
"QUANTITATION" {
    yylloc->step();
    return token::QUANTITATION;
}
"REPORT" {
    yylloc->step();
    return token::REPORT;
}
"REPTYPE" {
    yylloc->step();
    return token::REPTYPE;
}

"SEARCH" {
    yylloc->step();
    return token::SEARCH;
}

"TAXONOMY" {
    yylloc->step();
    return token::TAXONOMY;
}

"USER"[0-9][0-9] {
    yylloc->step();
    return token::USER;
}

"USEREMAIL" {
    yylloc->step();
    return token::USEREMAIL;
}

"and "/[0-9] {
    yylloc->step();
    return token::AND;
}

"USERNAME" {
    yylloc->step();
    return token::USERNAME;
}

"DECOY" {
    yylloc->step();
    return token::DECOY;
}

"END IONS"[ \r\t]* {
    yylloc->step();
    return token::END_IONS;
}

"PEPMASS" {
    yylloc->step();
    return token::PEPMASS;
}

"ERRORTOLERANT" {
    yylloc->step();
    return token::ERRORTOLERANT;
}

"PEP_ISOTOPE_ERROR" {
    yylloc->step();
    return token::PEP_ISOTOPE_ERROR;
}

"PFA" {
    yylloc->step();
    return token::PFA;
}

"PRECURSOR" {
    yylloc->step();
    return token::PRECURSOR;
}

"SEG" {
    yylloc->step();
    return token::SEG;
}

"FRAMES" {
    yylloc->step();
    return token::FRAMES;
}

"CHARGE" {
    yylloc->step();
    return token::CHARGE;
}

"COMP" {
    yylloc->step();
    return token::COMP;
}

"COM" {
    yylloc->step();
    return token::COM;
}

"ETAG" {
    yylloc->step();
    return token::ETAG;
}

"INSTRUMENT" {
    yylloc->step();
    return token::INSTRUMENT;
}

"IT_MODS" {
    yylloc->step();
    return token::IT_MODS;
}

"RTINSECONDS" {
    yylloc->step();
    return token::RTINSECONDS;
}

"SCANS" {
    yylloc->step();
    return token::SCANS;
}

"SEQ" {
    yylloc->step();
    return token::SEQ;
}

"TAG" {
    yylloc->step();
    return token::TAG;
}

"TOLU" {
    yylloc->step();
    return token::TOLU;
}

"TOL" {
    yylloc->step();
    return token::TOL;
}

 /* custom fields added to comply with GNPS' MGF format */
"MSLEVEL" {
    yylloc->step();
    return token::MSLEVEL;
}

"SOURCE_INSTRUMENT" {
    yylloc->step();
    return token::INSTRUMENT;
}

"FILENAME" {
    yylloc->step();
    return token::FILENAME;
}

"IONMODE" {
    yylloc->step();
    return token::IONMODE;
}

"ORGANISM" {
    yylloc->step();
    return token::ORGANISM;
}

"NAME="[^\n]* {
    std::string title(yytext);
    size_t pos = title.find("=");
    yylval->stringVal = new std::string();
    if (pos != std::string::npos && pos < (title.size()-1)) {
        *(yylval->stringVal) = title.substr(pos+1);
    }
    return token::TITLE;
}
 /* end custom fields */

#[^\n]* {
    yylloc->step();
    return token::COMMENT;
}

"TITLE="[^\n]* {
    std::string title(yytext);
    size_t pos = title.find("=");
    yylval->stringVal = new std::string();
    if (pos != std::string::npos && pos < (title.size()-1)) {
        *(yylval->stringVal) = title.substr(pos+1);
    }
    return token::TITLE;
}

[A-Za-z*][ :;,()A-Za-z0-9*_.-]* {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::STRING;
}

 /* gobble up white-spaces */
[ \t\r]+ {
    yylloc->step();
}

 /* gobble up lines of whitespace */
^[ \t\r]*\n {
    yylloc->lines(yyleng); yylloc->step();
}

 /* gobble up end-of-lines */
\n {
    yylloc->lines(yyleng); yylloc->step();
    return token::EOL;
}

 /* pass all other characters up to bison */
. {
    return static_cast<token_type>(*yytext);
}

 /*** END EXAMPLE - Change the example lexer rules above ***/

%% /*** Additional Code ***/

namespace mgf {

Scanner::Scanner(std::istream* in,
		 std::ostream* out)
    : MgfFlexLexer(in, out)
{
}

Scanner::~Scanner()
{
}

void Scanner::set_debug(bool b)
{
    yy_flex_debug = b;
}

}

/* This implementation of ExampleFlexLexer::yylex() is required to fill the
 * vtable of the class ExampleFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Scanner class instead.
#ifdef yylex
#undef yylex
#endif

int MgfFlexLexer::yylex()
{
    return 0;
}

*/

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */
int MgfFlexLexer::yywrap()
{
    return 1;
}

