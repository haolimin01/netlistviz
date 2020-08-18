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
#include <QPointF>
 
class SchematicDevice;
class ASG;


class CktNode
{
public:
    explicit CktNode(QString nodeName);
    ~CktNode();

    void    AddDevice(SchematicDevice *device);
    void    SetId(int id) { m_id = id; }
    int     Id() const { return m_id; }
    void    SetGnd(bool isGnd) { m_isGnd = isGnd; }
    void    ClearAllDevices();
    void    Print() const;
    bool    IsGnd() const { return m_isGnd; }
    QString Name() const { return m_name; }

    void    SetScenePos(const QPointF &pos, SchematicDevice *device);
    QPointF ScenePos() const { return m_pos; }

private:
    DISALLOW_COPY_AND_ASSIGN(CktNode);

    QString          m_name;
    int              m_id;
    bool             m_isGnd;
    QPointF          m_pos;
    SchematicDevice *m_posDevice; // this device's terminal pos.

    QVector<SchematicDevice *>  m_devices;

    friend class ASG;
};

#endif // NETLISTVIZ_CIRCUIT_CKTNODE_H
