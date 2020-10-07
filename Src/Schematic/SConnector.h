#ifndef NETLISTVIZ_SCHEMATIC_SCONNECTOR_H
#define NETLISTVIZ_SCHEMATIC_SCONNECTOR_H

/*
 * @filename : SConnector.h
 * @author   : Hao Limin
 * @date     : 2020.10.07
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : This structure is used to describe Connect Information between Schematic Devices.
 */

class SchematicDevice;
class SchematicTerminal;

struct SConnector
{
    SchematicTerminal *thisTerminal;
    SchematicTerminal *connectTerminal;
    SchematicDevice   *connectDevice;

    SConnector()
        : thisTerminal(nullptr), connectTerminal(nullptr), connectDevice(nullptr) {}
    SConnector(SchematicTerminal *thisTer, SchematicTerminal *cntTer, SchematicDevice *cntDev)
        : thisTerminal(thisTer), connectTerminal(cntTer), connectDevice(cntDev) {}
    ~SConnector() {}
};

#endif //  NETLISTVIZ_SCHEMATIC_SCONNECTOR_H
