#include "MyParser.h"
#include <iostream>
#include "Define/Define.h"

extern FILE *yyin;


MyParser::MyParser()
{
    m_cktParser = nullptr;
    yyin = NULL;
}


MyParser::~MyParser()
{
    if (m_cktParser) {
        delete m_cktParser;
        m_cktParser = nullptr;
    }

    if (yyin) {
        fclose(yyin);
        yyin = NULL;
    }
}


int MyParser::ParseNetlist(char *netlist)
{
#ifdef TRACE
    std::cout << LINE_INFO << std::endl;
#endif

    yyin = fopen(netlist, "r");
    if (!yyin) {
        printf("Open %s failed.\n", netlist);
        fclose(yyin);
        return ERROR;
    }

    if (m_cktParser) {
        delete m_cktParser;
    }

    m_cktParser = new yy::CktParser();
    m_cktParser->parse();

    fclose(yyin);
    return OKAY;
}