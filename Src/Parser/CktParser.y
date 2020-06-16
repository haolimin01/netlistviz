%language "C++"
%require "3.2"
%defines
%locations

%define parse.error verbose

%define api.parser.class {CktParser}

%{
    /*************** parser using bison ***************/
    /*************** Parser for Netlist ***************/
    #include <stdio.h>
    #include <iostream>
    #include <vector>
    #include "Define/Define.h"

    using std::cout;
    using std::endl;
    using std::vector;

    extern FILE *yyin;
    vector<char*> nodes;
%}

%output "CktParser.cpp"
%verbose

%union {
    int n;
    double f;
    char *s;
}

%token<f> VALUE
%token<s> STRING
%token<s> RESISTOR ISOURCE COMMENT
%token<n> INTEGER

%type<s> node
%type<f> value
%type<n> nodeList

%token DOTOP DOTEND EOL

%{
    extern int yylex(yy::CktParser::semantic_type *yylval, yy::CktParser::location_type *yylloc);
%}

%initial-action {
    @$.begin.filename = @$.end.filename = new std::string("stdin");
}

%%

spice: netlist end
;

end: DOTEND
   | end EOL
;

netlist: line
       | netlist line
;

line: component EOL
    | analysis EOL
    | ignore EOL
    | EOL
;

component: resistor
         | isource
;

analysis: op
;

ignore: COMMENT
;

resistor: RESISTOR nodeList value
        {
            printf("Resistor %s\n", $1);
        }
;

isource: ISOURCE nodeList value
       {
           printf("ISource %s\n", $1);
       }
;

op: DOTOP
;

nodeList: node
        {
            $$ = 1;
            nodes.push_back($1);
        }
        | nodeList node
        {
            $$ = $$ + 1;
            nodes.push_back($2);
        }
;

node: STRING
    {
        $$ = new char[strlen($1) + 1];
        strcpy($$, $1);
        $$[strlen($1)] = '\0';
    }
    | INTEGER
    {
        char *s = new char[20];
        sprintf(s, "%d", $1);
        $$ = new char[strlen(s) + 1];
        strcpy($$, s);
        $$[strlen(s)] = '\0';
        delete []s;
    }
;

value: INTEGER
     {
         $$ = $1;
     }
     | VALUE
     {
         $$ = $1;
     }
;

%%

namespace yy
{
    void CktParser::error(const location_type &loc, const std::string &s)
    {
        std::cerr << "Error : " << s << std::endl;
        EXIT;
    }
}

