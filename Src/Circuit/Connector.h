#ifndef NETLISTVIZ_CIRCUIT_CONNECTOR_H
#define NETLISTVIZ_CIRCUIT_CONNECTOR_H

/*
 * @filename : Connector.h
 * @date     : 2020.09.23
 * @author   : Hao Limin
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : This structure is used to describe Connect Information between devices.
 */

class Device;
class Terminal;

struct Connector
{
    Terminal  *thisTerminal;
    Terminal  *connectTerminal;
    Device    *connectDevice;

    Connector()
        : thisTerminal(nullptr), connectTerminal(nullptr), connectDevice(nullptr) {}

    Connector(Terminal *thisTer, Terminal *cntTer, Device *cntDev)
        : thisTerminal(thisTer), connectTerminal(cntTer), connectDevice(cntDev) {} 

    ~Connector() {}

};

#endif //NETLISTVIZ_CIRCUIT_CONNECTOR_H
