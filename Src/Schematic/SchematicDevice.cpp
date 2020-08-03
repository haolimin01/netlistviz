#include "SchematicDevice.h"
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <iostream>
#include <sstream>
#include "Circuit/CktNode.h"
#include "Define/Define.h"
#include "SchematicWire.h"


const int TerminalSize = 8;
const int IMAG_LEN = 25;
const int BASE_LEN = 20;
const int BRECT_W = 32;


SchematicDevice::SchematicDevice(DeviceType type, QMenu *contextMenu,
                                QTransform itemTransform, QGraphicsItem *parent)
{
    m_deviceType = type;
    m_contextMenu = contextMenu;
    setTransform(itemTransform);
    m_color = Qt::black;
    setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    switch (m_deviceType) {
        case Resistor:
            m_devOrien = Horizontal;
            DrawResistor();
            break;
        case Capacitor:
            m_devOrien = Horizontal;
            DrawCapacitor();
            break;
        case Inductor:
            m_devOrien = Horizontal;
            DrawInductor();
            break;
        case Isrc:
            m_devOrien = Vertical; 
            DrawIsrc();
            break;
        case Vsrc:
            m_devOrien = Vertical;
            DrawVsrc();
            break;
        default:;
    }

    m_imag = nullptr;
    m_showNodeFlag = false;
    m_wiresAtTerminal.resize(m_terNumber);
    m_id = -1;
    m_idGiven = false;

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    setZValue(0);
}

SchematicDevice::~SchematicDevice()
{
    if (m_imag)  delete m_imag;
}

QPixmap SchematicDevice::Image()
{
    if (m_imag)  return m_imag->copy();

    m_imag = new QPixmap(2 * IMAG_LEN, 2 * IMAG_LEN);
    m_imag->fill(Qt::white);

    QPainter painter(m_imag);
    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(IMAG_LEN, IMAG_LEN);
    painter.drawPath(path());

    return m_imag->copy();
}

int SchematicDevice::NodeId(NodeType type) const
{
    CktNode *node = m_terminals.value(type, nullptr);
    if (node)  return node->Id();
    else  return -1;
}

SchematicDevice::NodeType SchematicDevice::GetNodeType(CktNode *node) const
{
    NodeType type = m_terminals.key(node, Positive);
    return type;
}

void SchematicDevice::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    // do nothing now.
}

QRectF SchematicDevice::boundingRect() const
{
    return QRectF(-BRECT_W, -BRECT_W, 2 * BRECT_W, 2 * BRECT_W);
}

QRectF SchematicDevice::DashRect() const
{
    int halfTerSize = TerminalSize / 2;

    return QRectF(-12, -BASE_LEN + halfTerSize, 24, (BASE_LEN - halfTerSize) * 2);
}

void SchematicDevice::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    painter->setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setRenderHint(QPainter::Antialiasing);

    painter->drawPath(path());

    if (m_showNodeFlag) {
        painter->setBrush(QBrush(m_color));
        painter->setPen(Qt::NoPen);
        painter->drawRects(m_terRects);
    }

    if (isSelected()) {

        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(DashRect());
    }
}

void SchematicDevice::DrawResistor()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    QPainterPath path;

    m_name = "R";
    m_value = 1e3; // 1K

    int halfTerSize = TerminalSize / 2;

    path.moveTo(0, -BASE_LEN - halfTerSize);
    path.lineTo(0, -12);

    path.lineTo(-8, -10);
    path.lineTo(8, -6);
    path.lineTo(-8, -2);
    path.lineTo(8, 2);
    path.lineTo(-8, 6);
    path.lineTo(8, 10);

    path.lineTo(0, 12);
    path.lineTo(0, +BASE_LEN + halfTerSize);

    setPath(path);

    m_terNumber = 2;

    int width = TerminalSize;
    int upperCornerX = -halfTerSize;
    int upperCornerY = -BASE_LEN - TerminalSize;
    m_terRects.append(QRectF(upperCornerX, upperCornerY, width, width));

    int lowerCornerX = - halfTerSize;
    int lowerCornerY = BASE_LEN;
    m_terRects.append(QRectF(lowerCornerX, lowerCornerY, width, width));
}

void SchematicDevice::DrawCapacitor()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
    QPainterPath path;

    m_name = "C";
    m_value = 1e-12;  // 1P

    int halfTerSize = TerminalSize / 2;
    int dispV = 4;  // vertical displacement of the cap plate
    int refX = 10;  // x-coord for drawing cap plate

    // top vertical line
    path.moveTo(0, -BASE_LEN - halfTerSize);
    path.lineTo(0, -dispV);
    path.moveTo(-refX, -dispV);
    path.lineTo(refX, -dispV);
    path.moveTo(-refX, dispV);
    path.lineTo(refX, dispV);
    path.moveTo(0, dispV);
    path.lineTo(0, +BASE_LEN + halfTerSize);

    setPath(path);

    m_terNumber = 2;

    int width = TerminalSize;
    int upperCornerX = -halfTerSize;
    int upperCornerY = -BASE_LEN - TerminalSize;
    m_terRects.append(QRectF(upperCornerX, upperCornerY, width, width));
    int lowerCornerX = -halfTerSize;
    int lowerCornerY = BASE_LEN;
    m_terRects.append(QRectF(lowerCornerX, lowerCornerY, width, width));
}

void SchematicDevice::DrawInductor()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
    QPainterPath path;

    m_name = "L";
    m_value = 1e-3;  // 1mH

    int halfTerSize = TerminalSize / 2;

    path.moveTo(0, - BASE_LEN - halfTerSize);
    path.lineTo(0, -15);
    path.arcTo(-8, -15, 16, 10, 90, -240);
    path.arcTo(-8, -10, 16, 12, 150, -300);
    path.arcTo(-8, -3, 16, 12, 150, -300);
    path.arcTo(-8, 4, 16, 10, 150, -240);

    path.lineTo(0, +BASE_LEN + halfTerSize);

    setPath(path);

    m_terNumber = 2;
    int width = TerminalSize;
    int upperCornerX = -halfTerSize;
    int upperCornerY = -BASE_LEN - TerminalSize;
    m_terRects.append(QRectF(upperCornerX, upperCornerY, width, width));
    int lowerCornerX = -halfTerSize;
    int lowerCornerY = BASE_LEN;
    m_terRects.append(QRectF(lowerCornerX, lowerCornerY, width, width));
}

void SchematicDevice::DrawIsrc()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
    m_name = "I";
    m_value = 1e-3; // 1mA

    int cl = -12; // circle left coord
    int ct = -12; // circle top coord
    int cd = 24;  // circle diameter
    int cb = 12;  // circle bottom coord
    int tipY1 = -7; // arrow tip 1
    int tipY2 = -5; // arrow tip 2
    int tailY = 6;  // arrow tail

    QPainterPath path;
    int halfTerSize = TerminalSize / 2;

    path.moveTo(0, -BASE_LEN - halfTerSize);
    path.lineTo(0, ct);
    
    path.addEllipse(cl, ct, cd, cd);

    path.moveTo(0, tailY);
    path.lineTo(0, tipY2);
    path.moveTo(0, tipY1);
    path.lineTo(-2, -2);
    path.lineTo(2, -2);
    path.lineTo(0, tipY1);

    path.moveTo(0, cb);
    path.lineTo(0, BASE_LEN + halfTerSize);
    setPath(path);

    m_terNumber = 2;

    int width = TerminalSize;
    m_terRects.append(QRectF(-halfTerSize, -BASE_LEN-TerminalSize, width, width));
    m_terRects.append(QRectF(-halfTerSize, BASE_LEN, width, width));
}

void SchematicDevice::DrawVsrc()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
    m_name = "V";
    m_value = 1;  // 1V

    int cl = -12; // circle left coord
    int ct = -12; // circle top coord
    int cd = 24;  // circle diameter
    int cb = 12;  // circle bottom coord
    int signl = -2; // sign left
    int signr = 2;  // sign right
    int upsignref = -5; // upper sign ref coord
    int lowsignref = 5; // lower sign ref coord 

    QPainterPath path;
    int halfTerSize = TerminalSize / 2;

    path.moveTo(0, -BASE_LEN - halfTerSize);
    path.lineTo(0, ct);
    
    path.addEllipse(cl, ct, cd, cd);

    path.moveTo(signl, upsignref);
    path.lineTo(signr, upsignref);
    path.moveTo(0, upsignref - 2);
    path.lineTo(0, upsignref + 2);
    path.moveTo(signl, lowsignref);
    path.lineTo(signr, lowsignref);
    
    path.moveTo(0, cb);
    path.lineTo(0, BASE_LEN + halfTerSize);
    setPath(path);

    m_terNumber = 2;

    int width = TerminalSize;
    m_terRects.append(QRectF(-halfTerSize, -BASE_LEN-TerminalSize, width, width));
    m_terRects.append(QRectF(-halfTerSize, BASE_LEN, width, width));
}

QVector<QRectF> SchematicDevice::TerminalRects() const
{
    return m_terRects;
}

void SchematicDevice::AddNode(NodeType type, CktNode *node)
{
    m_terminals.insert(type, node);
}

CktNode* SchematicDevice::Node(NodeType type) const
{
    CktNode *node = nullptr;
    node = m_terminals.value(type, nullptr);
    return node;
}

QVariant SchematicDevice::itemChange(GraphicsItemChange change, const QVariant &value)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
    if (change == ItemPositionChange && scene()) {
        UpdateWirePosition();
    }
    
    return QGraphicsItem::itemChange(change, value);
}

void SchematicDevice::UpdateWirePosition()
{
    int i = 0;
    while (i < m_wiresAtTerminal.size()) {
        foreach(SchematicWire *wire, m_wiresAtTerminal[i]) {
            if (NOT wire->isSelected())
                wire->UpdatePosition(this, i, m_terRects[i].center());
        }
        i++;   
    }
}

void SchematicDevice::AddWire(SchematicWire *wire, int terIndex)
{
    assert(terIndex < m_terNumber);
    m_wiresAtTerminal[terIndex].append(wire);
}

void SchematicDevice::RemoveWires(bool deletion)
{
    int i = 0;
    int terIndex = 0;
    SchematicDevice *device = nullptr;
    while (i < m_wiresAtTerminal.size()) {
        foreach(SchematicWire *wire, m_wiresAtTerminal.at(i)) {
           scene()->removeItem(wire);
           device = wire->StartDevice();
           terIndex = wire->StartTerminalIndex();
           device->RemoveWire(wire, terIndex);
           device = wire->EndDevice();
           terIndex = wire->EndTerminalIndex();
           device->RemoveWire(wire, terIndex);
           if (deletion)  delete wire;
        }
        i++;
    }
}

void SchematicDevice::RemoveWire(SchematicWire *wire, int terIndex)
{
    assert(terIndex < m_terNumber);
    int wireIndex = m_wiresAtTerminal.at(terIndex).indexOf(wire);
    if (wireIndex != -1)
        m_wiresAtTerminal[terIndex].removeAt(wireIndex);
}

void SchematicDevice::Print() const
{
#if 0
    qInfo().noquote().nospace() << m_name << " type(" << m_deviceType << ") posName("
            << m_posNode->Name() << ") negName("
            << m_negNode->Name() << ") value(" << m_value << ")" << endl;
#endif
    std::stringstream ss;

    ss << "Name(" << m_name.toStdString() << "), ";
    ss << "type(" << m_deviceType << "), ";
    ss << "id(" << m_id << "), ";

    CktNode *node = nullptr;
    
    switch (m_deviceType) {
        case Resistor:
        case Capacitor:
        case Inductor:
        case Isrc:
        case Vsrc:
            node = Node(SchematicDevice::Positive);
            if (node) {
                ss << "posName(" << node->Name().toStdString() << "), ";
            }
            node = Node(SchematicDevice::Negative);
            if (node) {
                ss << "negName(" << node->Name().toStdString() << ")";
            }
            break;
        default:;
    }

    std::cout << ss.str() << std::endl;
 }

