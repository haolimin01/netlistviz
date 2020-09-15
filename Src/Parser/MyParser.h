#ifndef NETLISTVIZ_PARSER_MYPARSER_H
#define NETLISTVIZ_PARSER_MYPARSER_H

#include <fstream>
#include <string>
#include "Circuit/CircuitGraph.h"
#include "CktParser.hpp"

class MyParser
{
public:
    MyParser();
    ~MyParser();

    int ParseNetlist(const std::string &netlist, CircuitGraph *ckt);

private:
    yy::CktParser *m_cktParser;
};

#endif // NETLISTVIZ_PARSER_MYPARSER_H