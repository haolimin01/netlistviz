%option noyywrap

%{
    /*************** lexer using flex ***************/
    /************** Scanner for Netlist *************/
    #include <stdio.h>
    #include <stdlib.h>
    #include "Circuit/CircuitGraph.h"
    #include "CktParser.hpp"
    #include "Define/Define.h"

    #define YY_DECL int yylex(yy::CktParser::semantic_type *yylval, yy::CktParser::location_type *yylloc)
    #define YY_USER_ACTION yylloc->columns(yyleng);
    
    double CvtStrToValue(char *str);

    typedef yy::CktParser::token token;

    int lineno = 0;
%}

%option outfile="CktScanner.cpp"


ALPHA       [A-Za-z_]
DIGIT       [0-9]
ALPHANUM    [A-Za-z_0-9]
STRING      {ALPHANUM}+
INTEGER     [-+]?{DIGIT}+
FLOAT       [-+]?{DIGIT}*\.{DIGIT}+([Ee][-+]?{DIGIT}+)?
UNIT        ([Ff]|[Pp]|[Nn]|[Uu]|[Mm]|[Kk]|[Mm][Ee][Gg]|[Gg]|[Tt])\s*([Hh][Zz]|[Ss]|[Hh])?
VALUE       ({INTEGER}|{FLOAT})\s*{UNIT}?

CAPACITOR   ^[Cc]{STRING}
ISOURCE     ^[Ii]{STRING}
INDUCTOR    ^[Ll]{STRING}
RESISTOR    ^[Rr]{STRING}
VSOURCE     ^[Vv]{STRING}

AC          [Aa][Cc]
DC          [Dd][Cc]
TRAN        [Tt][Rr][Aa][Nn]

V           [Vv] 
VM          [Vv][Mm]
VP          [Vv][Pp]
VR          [Vv][Rr]
I           [Ii]

DOTPRINT    ^[\.][Pp][Rr][Ii][Nn][Tt]
DOTPLOT     ^[\.][Pp][Ll][Oo][Tt]
DOTOP       ^[\.][Oo][Pp]

COMMENT     ^\*.*?$
EOL         [\n\r]+
DELIMITER   [ \t]+
LP          \(
RP          \)
COMMA       ,
DOTEND      ^[\.][Ee][Nn][Dd][Ss]?

%%

%{
    // start where previous token ended
    yylloc->step();
%}

{INTEGER}       {yylval->n = atoi(yytext); return token::INTEGER;}
{VALUE}         {yylval->f = CvtStrToValue(yytext); return token::VALUE;}

{CAPACITOR}     {
                yylval->s = (char*)malloc((strlen(yytext)+1) * sizeof(char));
                strcpy(yylval->s, yytext);
                yylval->s[strlen(yytext)] = '\0';
                return token::CAPACITOR;
                }

{ISOURCE}       {
                yylval->s = (char*)malloc((strlen(yytext)+1) * sizeof(char));
                strcpy(yylval->s, yytext);
                yylval->s[strlen(yytext)] = '\0';
                return token::ISOURCE;
                }

{INDUCTOR}      {
                yylval->s = (char*)malloc((strlen(yytext)+1) * sizeof(char));
                strcpy(yylval->s, yytext);
                yylval->s[strlen(yytext)] = '\0';
                return token::INDUCTOR;
                }

{RESISTOR}      {
                yylval->s = (char*)malloc((strlen(yytext)+1) * sizeof(char));
                strcpy(yylval->s, yytext);
                yylval->s[strlen(yytext)] = '\0';
                return token::RESISTOR;
                }

{VSOURCE}       {
                yylval->s = (char*)malloc((strlen(yytext)+1) * sizeof(char));
                strcpy(yylval->s, yytext);
                yylval->s[strlen(yytext)] = '\0';
                return token::VSOURCE;
                }

{AC}            {return token::AC;}
{DC}            {return token::DC;}
{TRAN}          {return token::TRAN;}

{V}             {
                yylval->s = (char*)malloc(2 * sizeof(char));
                yylval->s[0] = 'V';
                yylval->s[1] = '\0';
                return token::VTYPE;
                }
                
{VM}            {
                yylval->s = (char*)malloc(3 * sizeof(char));
                yylval->s[0] = 'V';
                yylval->s[1] = 'M';
                yylval->s[2] = '\0';
                return token::VTYPE;
                }

{VP}            {
                yylval->s = (char*)malloc(3 * sizeof(char));
                yylval->s[0] = 'V';
                yylval->s[1] = 'P';
                yylval->s[2] = '\0';
                return token::VTYPE;
                }

{VR}            {
                yylval->s = (char*)malloc(3 * sizeof(char));
                yylval->s[0] = 'V';
                yylval->s[1] = 'R';
                yylval->s[2] = '\0';
                return token::VTYPE;
                }

{I}             {return token::I;}

{DOTPRINT}      {return token::DOTPRINT;}

{DOTPLOT}       {return token::DOTPLOT;}

{DOTOP}         {return token::DOTOP;}

{COMMENT}       {
                yylval->s = (char*)malloc((strlen(yytext)+1) * sizeof(char));
                strcpy(yylval->s, yytext);
                yylval->s[strlen(yytext)] = '\0';
                return token::COMMENT;
                }

{STRING}        {
                yylval->s = (char*)malloc((strlen(yytext)+1) * sizeof(char));
                strcpy(yylval->s, yytext);
                yylval->s[strlen(yytext)] = '\0';
                return token::STRING;
                }

{EOL}           {
                // lineno++;
                // printf("line number : %d\n", lineno);
                return token::EOL;
                }
{LP}            {return token::LP;}
{RP}            {return token::RP;}
{COMMA}         {return token::COMMA;}

{DOTEND}        {return token::DOTEND;}
{DELIMITER}     {}


<<EOF>>         {yyterminate();}
.               {printf("UNKNOWN (%s)\n", yytext);}

%%

double CvtStrToValue(char *str)
{
    double value;
    value = atof(str);
    int len = strlen(str);
    char u;

    for (int i = len - 1; i >= 0; -- i) {
        u = str[i];
        if (u == 'S' || u == 's') continue;
        if (u == 'H' || u == 'h') continue;
        if (u == 'Z' || u == 'z') continue;

        if (u == 'F' || u == 'f')
            value *= 1e-15;
        else if (u == 'P' || u == 'p')
            value *= 1e-12;
        else if (u == 'N' || u == 'n')
            value *= 1e-9;
        else if (u == 'U' || u == 'u')
            value *= 1e-6;
        else if (u == 'M' || u == 'm')
            value *= 1e-3;
        else if (u == 'K' || u == 'k')
            value *= 1e3;
        else if (u == 'G' || u == 'g') {
            if (str[i - 1] == 'E' || str[i - 1] == 'e')
                value *= 1e6;
            else
                value *= 1e9;
        } else if (u >= '0' && u <= '9')
            break;
    }

    return value;
}
