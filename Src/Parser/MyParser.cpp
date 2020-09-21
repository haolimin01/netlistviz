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

int MyParser::ParseNetlist(const std::string &netlist, CircuitGraph *ckt)
{
    assert(ckt);
#ifdef TRACE
    std::cout << LINE_INFO << std::endl;
#endif
    yyin = fopen(netlist.c_str(), "r");
    if (NOT yyin) {
        std::cout << "Open " << netlist << " failed.\n" << std::endl;
        fclose(yyin);
        return ERROR;
    }

    if (m_cktParser) {
        delete m_cktParser;
    }

    m_cktParser = new yy::CktParser(ckt);
    m_cktParser->parse();

    fclose(yyin);
    yyin = NULL;
    return OKAY;
}