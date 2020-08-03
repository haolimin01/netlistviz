#ifndef NETLISTVIZ_CIRCUIT_CKTNODE_H
#define NETLISTVIZ_CIRCUIT_CKTNODE_H

/*
 * @filename : CktNode.h
 * @author   : Hao Limin
 * @date     : 2020.07.25
 * @desp     : Circuit Node, do NOT need to render on scene
 */

#include "Define/Define.h"
#include <QString>
#include <QVector>
 
class SchematicDevice;
class ASG;


class CktNode
{
public:
    explicit CktNode(QString nodeName);
    ~CktNode();

    void AddDevice(SchematicDevice *device);
    void SetId(int id) { m_id = id; }
    int  Id() const { return m_id; }
    void SetGnd(bool isGnd) { m_isGnd = isGnd; }
    void ClearAllDevices();
    void Print() const;
    bool IsGnd() const { return m_isGnd; }
    QString Name() const { return m_name; }

private:
    DISALLOW_COPY_AND_ASSIGN(CktNode);

    QString     m_name;
    int         m_id;
    bool        m_isGnd;

    QVector<SchematicDevice *>  m_devices;

    friend class ASG;
};

#endif // NETLISTVIZ_CIRCUIT_CKTNODE_H
