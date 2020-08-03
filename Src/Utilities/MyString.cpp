#include "MyString.h"
#include <cstring>
#include <QString>


std::string CppString(QString qStr)
{
    return qStr.toStdString();
}

std::string CppString(char *cStr)
{
    return std::string(cStr);
}

const char* CString(QString qStr)
{
    return qStr.toStdString().c_str();
}

const char* CString(std::string cppStr)
{
    return cppStr.c_str();
}

QString qString(char *cStr)
{
    return QString::fromLatin1(cStr);
}

QString qString(std::string cppStr)
{
    return QString::fromStdString(cppStr);
}