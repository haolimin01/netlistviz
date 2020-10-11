#ifndef NETLIST_UTILITIES_STRING_H
#define NETLIST_UTILITIES_STRING_H

#include <string>

class QString;

extern std::string CppString(QString qStr);
extern std::string CppString(char *cStr);

extern const char* CString(QString qStr);
extern const char* CString(std::string cppStr);

extern QString     qString(char *cStr);
extern QString     qString(std::string cppStr);


#endif // NETLIST_UTILITIES_STRING_H
