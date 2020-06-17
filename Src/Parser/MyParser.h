#ifndef NETLISTVIZ_PARSER_MYPARSER_H
#define NETLISTVIZ_PARSER_MYPARSER_H

#include <fstream>
#include <string>
#include "Schematic/SchematicData.h"
#include "CktParser.hpp"


class MyParser
{
public:
    MyParser();
    ~MyParser();

    int ParseNetlist(const std::string &netlist, SchematicData *data);

private:
    yy::CktParser *m_cktParser;
};

#endif // NETLISTVIZ_PARSER_MYPARSER_H