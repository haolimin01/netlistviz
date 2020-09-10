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
const int BRECT_W = 30;


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
            m_devOrien = Vertical;
            m_priority = R_Priority;
            DrawResistor();
            m_isDevice = true;
            break;
        case Capacitor:
            m_devOrien = Vertical;
            m_priority = C_Priority;
            DrawCapacitor();
            m_isDevice = true;
            break;
        case Inductor:
            m_devOrien = Vertical;
            m_priority = L_Priority;
            DrawInductor();
            m_isDevice = true;
            break;
        case Isrc:
            m_devOrien = Vertical; 
            m_priority = I_Priority;
            DrawIsrc();
            m_isDevice = true;
            break;
        case Vsrc:
            m_devOrien = Vertical;
            m_priority = V_Priority;
            DrawVsrc();
            m_isDevice = true;
            break;
        // case Dot:
        //     m_devOrien = Vertical;
        //     m_priority = Dot_Priority;
        //     DrawDot();
        //     m_isDevice = false;
        //     break;
        case GND:
            m_devOrien = Vertical;
            m_priority = GND_Priority;
            DrawGND();
            m_isDevice = false;
            break;
        default:;
    }

    m_imag = nullptr;
    m_showNodeFlag = false;
    m_id = -1;
    m_idGiven = false;
    m_sceneX = -1;
    m_sceneY = -1;
    m_placed = false;
    m_onBranch = false;
    m_showOnBranchFlag = false;
    m_maybeAtFirstLevel = false;
    
    CreateAnnotation(m_name);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    setZValue(0);
}

SchematicDevice::~SchematicDevice()
{
    if (m_imag)  delete m_imag;
    if (m_annotText) delete m_annotText;
}

QPixmap SchematicDevice::Image()
{
    if (m_imag)  return m_imag->copy();

    m_imag = new QPixmap(2 * IMAG_LEN, 2 * IMAG_LEN);
    m_imag->fill(Qt::transparent);

    QPainter painter(m_imag);

    // if (m_deviceType == Dot) {
    //     painter.setBrush(QBrush(Qt::black));
    // }

    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(IMAG_LEN, IMAG_LEN);
    painter.drawPath(path());

    return m_imag->copy();
}

int SchematicDevice::TerminalId(TerminalType type) const
{
    CktNode *node = m_terminals.value(type, nullptr);
    if (node)  return node->Id();
    else  return -1;
}

TerminalType SchematicDevice::GetTerminalType(CktNode *node) const
{
    TerminalType type = m_terminals.key(node, Positive);
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

QPainterPath SchematicDevice::shape() const
{
    QPainterPath path;

    // if (m_deviceType == Dot) {
    //     path.addEllipse(-2, -2, 4, 4);
    //     return path;
    // }
    
    QMap<TerminalType, QRectF>::const_iterator cit;
    for (cit = m_terRects.constBegin(); cit != m_terRects.constEnd(); ++ cit)
        path.addRect(cit.value());

    if (m_deviceType == GND) {
        int halfTerSize = TerminalSize / 2;
        path.addRect(QRectF(-12, -10 + halfTerSize, 24, (10 - halfTerSize) * 2));
    } else {
        path.addRect(DashRect());
    }

    return path;
}

QRectF SchematicDevice::DashRect() const
{
    int halfTerSize = TerminalSize / 2;

    switch (m_deviceType) {
        case Resistor:
        case Capacitor:
        case Inductor:
        case Isrc:
        case Vsrc:
            return QRectF(-12, -BASE_LEN + halfTerSize, 24, (BASE_LEN - halfTerSize) * 2);
        case GND:
            return QRect(-10, -4, 20, 16);
        // case Dot:
        //     return QRect(-8, -8, 16, 16);
        default:
            return QRect(-BASE_LEN, -BASE_LEN, BASE_LEN*2, BASE_LEN*2);
    }
}

void SchematicDevice::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    int lineWidth = 2;
    if (m_onBranch && m_showOnBranchFlag) {
        m_color = Qt::red;
        lineWidth = 3;
    } else {
        m_color = Qt::black;
    }

    // if (m_deviceType == Dot) {
    //     painter->setPen(Qt::NoPen);
    //     painter->setBrush(QBrush(m_color));
    // } else {
    //     painter->setPen(QPen(m_color, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // }

    painter->setPen(QPen(m_color, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    painter->setRenderHint(QPainter::Antialiasing);

    painter->drawPath(path());

    if (m_showNodeFlag) {
        painter->setBrush(QBrush(m_color));
        painter->setPen(Qt::NoPen);

        QMap<TerminalType, QRectF>::const_iterator cit;
        for (cit = m_terRects.constBegin(); cit != m_terRects.constEnd(); ++ cit)
            painter->drawRect(cit.value());
    }

    if (isSelected()) {

        painter->setPen(QPen(m_color, 1, Qt::DashLine));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(DashRect());
    }
}

void SchematicDevice::SetName(QString name)
{
    m_name = name;
    if (m_annotText)
        m_annotText->setHtml(m_name);
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
    m_terRects.insert(Positive, QRectF(upperCornerX, upperCornerY, width, width));

    int lowerCornerX = - halfTerSize;
    int lowerCornerY = BASE_LEN;
    m_terRects.insert(Negative, QRectF(lowerCornerX, lowerCornerY, width, width));
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
    m_terRects.insert(Positive, QRectF(upperCornerX, upperCornerY, width, width));

    int lowerCornerX = -halfTerSize;
    int lowerCornerY = BASE_LEN;
    m_terRects.insert(Negative, QRectF(lowerCornerX, lowerCornerY, width, width));
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
    m_terRects.insert(Positive, QRectF(upperCornerX, upperCornerY, width, width));

    int lowerCornerX = -halfTerSize;
    int lowerCornerY = BASE_LEN;
    m_terRects.insert(Negative, QRectF(lowerCornerX, lowerCornerY, width, width));
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
    m_terRects.insert(Positive, QRectF(-halfTerSize, -BASE_LEN-TerminalSize, width, width));
    m_terRects.insert(Negative, QRectF(-halfTerSize, BASE_LEN, width, width));
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
    m_terRects.insert(Positive, QRectF(-halfTerSize, -BASE_LEN-TerminalSize, width, width));
    m_terRects.insert(Negative, QRectF(-halfTerSize, BASE_LEN, width, width));
}

void SchematicDevice::DrawGND()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
/*
*      |
*      |
*    -----
*     ---
*      -
*/
    // m_name = "G";
    m_name = "";
    m_value = 0;

    QPainterPath path;
    int vWire = 10;   // Length of the vertical wire
    path.moveTo(0, -vWire);
    path.lineTo(0, 0);
    path.moveTo(-8, 0);
    path.lineTo(8, 0);
    path.moveTo(-6, 4);
    path.lineTo(6, 4);
    path.moveTo(-4, 8);
    path.lineTo(4, 8);
    setPath(path);

    m_terNumber = 1;
    
    // The terminal-rect is centered at (0, -vWire)
    QRectF rect(-TerminalSize/2, -vWire-TerminalSize/2, TerminalSize, TerminalSize);
    m_terRects.insert(General, rect);
}

/*
void SchematicDevice::DrawDot()
{
    m_name = "";
    m_value = 0;

    QPainterPath path;
    path.addEllipse(-4, -4, 8, 8);
    setPath(path);

    m_terNumber = 1;
    QRectF rect(-TerminalSize/2, -TerminalSize/2, TerminalSize, TerminalSize);
    m_terRects.insert(General, rect);
}
*/

void SchematicDevice::SetSceneXY(int x, int y)
{
    Q_ASSERT(x >= 0 && y >= 0);
    m_sceneX = x;
    m_sceneY = y;
}

void SchematicDevice::AddTerminal(TerminalType type, CktNode *node)
{
    m_terminals.insert(type, node);
}

CktNode* SchematicDevice::Terminal(TerminalType type) const
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
    if (NOT scene())
        return QGraphicsItem::itemChange(change, value);

    if (change == ItemPositionHasChanged) {
        UpdateWirePosition();
    }

    /* deal with annotation text */
    if (change == ItemSceneChange) {
        scene()->removeItem(m_annotText);
    } else if (change == ItemSceneHasChanged) {
        scene()->addItem(m_annotText);
        m_annotText->setPos(mapToScene(m_annotRelPos));
    } else if (change == ItemPositionHasChanged) {
        m_annotText->setPos(mapToScene(m_annotRelPos));
    }

    return QGraphicsItem::itemChange(change, value);
}

void SchematicDevice::UpdateWirePosition()
{
    QMap<TerminalType, QVector<SchematicWire*>>::const_iterator cit;
    cit = m_wiresAtTerminal.constBegin();
    for (; cit != m_wiresAtTerminal.constEnd(); ++ cit) {
        foreach (SchematicWire *wire, cit.value()) {
            //if (NOT wire->isSelected()) 
            wire->UpdatePosition(this, cit.key(), m_terRects[cit.key()].center());
        }
    }
}

void SchematicDevice::AddWire(SchematicWire *wire, TerminalType type)
{
    m_wiresAtTerminal[type].append(wire);
}

void SchematicDevice::RemoveWires(bool deletion)
{
    TerminalType terminal;
    SchematicDevice *device = nullptr;
    QMap<TerminalType, QVector<SchematicWire*>>::iterator cit;
    for (cit = m_wiresAtTerminal.begin(); cit != m_wiresAtTerminal.end(); ++ cit) {
        foreach (SchematicWire *wire, cit.value()) {
            scene()->removeItem(wire);
            device = wire->StartDevice();
            terminal = wire->StartTerminal();
            device->RemoveWire(wire, terminal);
            device = wire->EndDevice();
            terminal = wire->EndTerminal();
            device->RemoveWire(wire, terminal);
            if (deletion)  delete wire;
        }
    }
}

void SchematicDevice::RemoveWire(SchematicWire *wire, TerminalType type)
{
    int wireIndex = m_wiresAtTerminal[type].indexOf(wire);
    if (wireIndex != -1)
        m_wiresAtTerminal[type].removeAt(wireIndex);
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
            node = Terminal(Positive);
            if (node) {
                ss << "posName(" << node->Name().toStdString() << "), ";
            }
            node = Terminal(Negative);
            if (node) {
                ss << "negName(" << node->Name().toStdString() << ")";
            }
            break;
        default:;
    }

    std::cout << ss.str() << std::endl;
 }

 void SchematicDevice::SetOrientation(SchematicDevice::Orientation orien)
 {
    if (orien == m_devOrien)  return;
    if (orien == Horizontal) {
        setRotation(-90);
    } else {
        setRotation(90);
    }
    m_devOrien = orien;

    /* For annotation text */
    SetCustomAnnotRelPos();
    if (orien == Horizontal)
        m_annotRelPos.rx() += m_annotText->boundingRect().height() / 2;
    m_annotText->setPos(mapToScene(m_annotRelPos));
 }

 void SchematicDevice::mousePressEvent(QGraphicsSceneMouseEvent *event)
 {
    QGraphicsPathItem::mousePressEvent(event);
 }

bool SchematicDevice::TerminalsContain(const QPointF &scenePos) const
{
    QPointF itemPos = mapFromScene(scenePos);
    QMap<TerminalType, QRectF>::const_iterator cit;
    cit = m_terRects.constBegin();
    for (; cit != m_terRects.constEnd(); ++ cit) {
        if (cit.value().contains(itemPos))
            return true;
    }

    return false;
}

QPointF SchematicDevice::TerminalPos(TerminalType type) const
{
    return m_terRects[type].center();
}

QPointF SchematicDevice::TerminalScenePos(TerminalType type) const
{
    QPointF itemPos = m_terRects[type].center();
    return mapToScene(itemPos);
}

QPointF SchematicDevice::NodeScenePos(TerminalType type) const
{
    CktNode *node = Terminal(type);
    return node->ScenePos();
}

/*
void SchematicDevice::UpdateTerminalScenePos()
{
    CktNode *node = nullptr;
    switch (m_deviceType) {
        case Resistor:
        // case Capacitor:
        case Inductor:
        case Isrc:
        case Vsrc:
            node = Terminal(Positive);
            node->SetScenePos(TerminalScenePos(Positive));

            node = Terminal(Negative);
            node->SetScenePos(TerminalScenePos(Negative));
            break;
        default:;
    }
}
*/

void SchematicDevice::UpdateNodeScenePos()
{
    CktNode *node = nullptr;
    switch (m_deviceType) {
        case Resistor:
        case Inductor:
        case Isrc:
        case Vsrc:
            node = Terminal(Positive);
            node->SetScenePos(TerminalScenePos(Positive));

            node = Terminal(Negative);
            node->SetScenePos(TerminalScenePos(Negative));
            break;
        default:;
    }
}

QPointF SchematicDevice::ScenePosByTerminalScenePos(TerminalType type, const QPointF &scenePos)
{
    QPointF pos = TerminalPos(type);
    return scenePos - pos;
}

bool SchematicDevice::TerminalsContainBranchWire()
{
    QMap<TerminalType, QVector<SchematicWire*>>::const_iterator cit;
    cit = m_wiresAtTerminal.constBegin();
    for (; cit != m_wiresAtTerminal.constEnd(); ++ cit) {
        foreach (SchematicWire *wire, cit.value()) {
            if (wire->IsBranch())
                return true;
        }
    }

    return false;
}

void SchematicDevice::CreateAnnotation(const QString &text)
{
    m_annotText = new QGraphicsTextItem();
    m_annotText->setHtml(text);

    m_annotText->setFlag(QGraphicsItem::ItemIsMovable, true);
    m_annotText->setFlag(QGraphicsItem::ItemIsSelectable, true);

    m_annotText->setFont(QFont("Courier 10 Pitch", 10));

    m_annotText->setZValue(-1);

    SetCustomAnnotRelPos();
}

void SchematicDevice::SetCustomAnnotRelPos()
{
    int offsetX = 0, offsetY = 0;
    QRectF dRect, bRect;
    dRect = mapRectToScene(DashRect());
    bRect = m_annotText->boundingRect();

    offsetX = dRect.width()/2 + 1;
    offsetY = -bRect.height()/2;

    m_annotRelPos = QPointF(offsetX, offsetY);
}

bool SchematicDevice::GroundCap() const
{
    if (m_deviceType != Capacitor)
        return false;

    CktNode *posNode = Terminal(Positive);
    if (posNode->IsGnd())
        return true;

    CktNode *negNode = Terminal(Negative);
    if (negNode->IsGnd())
        return true;

    /* device isn't ground cap */
    return false;
}

bool SchematicDevice::CoupledCap() const
{
    if (m_deviceType != Capacitor)
        return false;

    CktNode *posNode = Terminal(Positive);
    if (posNode->IsGnd())
        return false;

    CktNode *negNode = Terminal(Negative);
    if (negNode->IsGnd())
        return false;

    /* device is coupled cap */
    return true;
}
