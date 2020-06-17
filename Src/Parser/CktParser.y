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
    #include "Schematic/SchematicData.h"

    using std::cout;
    using std::endl;
    using std::vector;
    using std::string;

    extern FILE *yyin;
    vector<char*> nodes;

    void PrintDevice(char*, const vector<char*> &, double);
%}

%output "CktParser.cpp"
%verbose

%parse-param { SchematicData *data }

%union {
    int n;
    double f;
    char *s;
}

%token<f> VALUE
%token<s> STRING
%token<s> CAPACITOR ISOURCE INDUCTOR RESISTOR VSOURCE COMMENT
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

component: capacitor
         | isource
         | inductor
         | resistor
         | vsource
;

analysis: op
;

ignore: COMMENT
;

capacitor: CAPACITOR nodeList value
         {
            if ($2 != 2 || nodes.size() != 2) {
                printf("Parse %s failed.\n", $1);
                EXIT;
            }

            data->ParseC($1, nodes.at(0), nodes.at(1), $3);

            nodes.clear();
         }
;

isource: ISOURCE nodeList value
       {
            if ($2 != 2 || nodes.size() != 2) {
                printf("Parse %s failed.\n", $1);
                EXIT;
            }

            data->ParseI($1, nodes.at(0), nodes.at(1), $3);

            nodes.clear();
       }
;

inductor: INDUCTOR nodeList value
        {
            if ($2 != 2 || nodes.size() != 2) {
                printf("Parse %s failed.\n", $1);
                EXIT;
            }

            data->ParseL($1, nodes.at(0), nodes.at(1), $3);

            nodes.clear();
        }

resistor: RESISTOR nodeList value
        {
            if ($2 != 2 || nodes.size() != 2) {
                printf("Parse %s failed.\n", $1);
                EXIT;
            }

            data->ParseR($1, nodes.at(0), nodes.at(1), $3);

            nodes.clear();
        }
;

vsource: VSOURCE nodeList value
       {
            if ($2 != 2 || nodes.size() != 2) {
                printf("Parse %s failed.\n", $1);
                EXIT;
            }

            data->ParseV($1, nodes.at(0), nodes.at(1), $3);

            nodes.clear();
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

void PrintDevice(char *name, const vector<char*> &nodes, double value)
{
    printf("%s ", name);
    for (size_t i = 0; i < nodes.size(); ++ i) {
        printf("%s ", nodes.at(i));
    }
    printf("%lf\n", value);
}
