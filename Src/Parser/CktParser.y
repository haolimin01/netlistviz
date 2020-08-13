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

    struct OutputPara
    {
        char *otype;  // output type (V, VM, VR, VP, I)
        char *pos;    // positive node
        char *neg;    // negative node
        OutputPara()  { otype = NULL; pos = NULL; neg = NULL; }
        OutputPara(char *o, char *p, char *n)
        {
            otype = new char[strlen(o) + 1];
            strcpy(otype, o);
            otype[strlen(o)] = '\0';

            pos = new char[strlen(p) + 1];
            strcpy(pos, p);
            pos[strlen(p)] = '\0';

            neg = new char[strlen(n) + 1];
            strcpy(neg, n);
            neg[strlen(n)] = '\0';
        }
    };

    extern FILE       *yyin;
    vector<char*>      nodes;
    vector<OutputPara> outs;

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
%token<s> VTYPE

%type<s> node
%type<f> value
%type<n> nodeList outList

%token DOTPRINT DOTPLOT LP RP COMMA DOTOP DOTEND EOL
%token I AC DC TRAN

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
    | output EOL
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

output: print
      | plot
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

print: DOTPRINT outList
     {
         outs.clear();
     }
     | DOTPRINT DC outList
     {
         outs.clear();
     }
     | DOTPRINT AC outList
     {
         outs.clear();
     }
     | DOTPRINT TRAN outList
     {
         outs.clear();
     }
;

plot: DOTPLOT outList
    {
        outs.clear();
    }
    | DOTPLOT DC outList
    {
        outs.clear();
    }
    | DOTPLOT AC outList
    {
        outs.clear();
    }
    | DOTPLOT TRAN outList
    {
        outs.clear();
    }
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

outList: VTYPE LP node RP
       {
           $$ = 1;
           outs.push_back(OutputPara($1, $3, "0"));
       }
       | VTYPE LP node COMMA node RP
       {
           $$ = $$ + 1;
           outs.push_back(OutputPara($1, $3, $5));
       }
       | I LP VSOURCE RP
       {
           outs.push_back(OutputPara("I", $3, "0"));
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
