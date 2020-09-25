#ifndef NETLISTVIZ_CIRCUIT_CONNECT_DESCRIPTOR_H
#define NETLISTVIZ_CIRCUIT_CONNECT_DESCRIPTOR_H

/*
 * @filename : ConnectDescriptor.h
 * @date     : 2020.09.23
 * @author   : Hao Limin
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : This structure is used to describe Connect Information between devices.
 */

class Device;
class Terminal;

struct ConnectDescriptor
{
    Terminal  *thisTerminal;
    Terminal  *connectTerminal;
    Device    *connectDevice;

    ConnectDescriptor()
        : thisTerminal(nullptr), connectTerminal(nullptr), connectDevice(nullptr) {}

    ConnectDescriptor(Terminal *thisTer, Terminal *cntTer, Device *cntDev)
        : thisTerminal(thisTer), connectTerminal(cntTer), connectDevice(cntDev) {} 

    ~ConnectDescriptor() {}

};

#endif //NETLISTVIZ_CIRCUIT_CONNECT_DESCRIPTOR_H
