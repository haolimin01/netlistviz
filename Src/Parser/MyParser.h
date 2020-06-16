#ifndef NETLISTVIZ_PARSER_MYPARSER_H
#define NETLISTVIZ_PARSER_MYPARSER_H

#include <fstream>
#include "CktParser.hpp"


class MyParser
{
public:
    MyParser();
    ~MyParser();

    int ParseNetlist(char *netlist);

private:
    yy::CktParser *m_cktParser;

};

#endif // NETLISTVIZ_PARSER_MYPARSER_H