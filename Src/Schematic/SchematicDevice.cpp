#include "SchematicDevice.h"
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QGraphicsScene>
#include "SchematicTerminal.h"
#include "SchematicWire.h"
#include "SConnector.h"

static const int TerminalSize = 8;
static const int IMAG_LEN = 25;
static const int BASE_LEN = 20;
static const int BRECT_W = 30;

SchematicDevice::SchematicDevice()
{
    m_contextMenu = nullptr;
    setTransform(QTransform());

    m_deviceType = RESISTOR;
    m_id = 0;
    m_name = "";
    m_reverse = false;
    m_geoCol = 0;
    m_geoRow = 0;
    m_devOrien = Vertical;
    m_annotText = new QGraphicsTextItem();

    /* ASG entrance */
    /* add terminals by AddTerminal() */
    /* then call Initialize() to draw device shape */
}

SchematicDevice::SchematicDevice(DeviceType type, QMenu *contextMenu,
    QTransform transform, QGraphicsItem *parent)
{
    m_contextMenu = contextMenu;

    m_deviceType = type;
    m_id = 0;
    m_name = "";
    m_reverse = false;
    m_geoCol = 0;
    m_geoRow = 0;
    m_devOrien = Vertical;
    m_annotText = new QGraphicsTextItem();
    setTransform(transform);

    /* Insert SchematicDevice entrance */
    /* create terminals by self */
    CreateTerminalsBySelf();

    Initialize();
}

SchematicDevice::~SchematicDevice()
{
    if (m_imag) delete m_imag;
    if (m_annotText) delete m_annotText;
    /* delete its terminals here */
    foreach (SchematicTerminal *ter, m_terminals.values())
        delete ter;
    m_terminals.clear();
    ClearConnectors();
}

void SchematicDevice::Initialize()
{
    m_color = Qt::black;
    setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    
    switch (m_deviceType) {
        case RESISTOR:
            DrawResistor();
            m_isDevice = true;
            break;
        case CAPACITOR:
            DrawCapacitor();
            m_isDevice = true;
            break;
        case INDUCTOR:
            DrawInductor();
            m_isDevice = true;
            break;
        case ISRC:
            DrawIsrc();
            m_isDevice = true;
            break;
        case VSRC:
            DrawVsrc();
            m_isDevice = true;
            break;
        case GND:
            DrawGnd();
            // DrawSmallGnd();
            m_isDevice = false;
            break;
        default:;
    }

    m_imag = nullptr;
    m_showTerminal = false;

    m_sceneCol = 0;
    m_sceneRow = 0;
    m_smallGnd = false;
    // CreateAnnotation(m_name);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    setZValue(0);
}

void SchematicDevice::CreateTerminalsBySelf()
{
    m_terminals.clear();
    SchematicTerminal *terminal = nullptr;
    switch (m_deviceType) {
        case RESISTOR:
        case CAPACITOR:
        case INDUCTOR:
        case ISRC:
        case VSRC:
            terminal = new SchematicTerminal(Positive, this);
            m_terminals.insert(Positive, terminal);
            terminal = new SchematicTerminal(Negative, this);
            m_terminals.insert(Negative, terminal);
            break;
        case GND:
            terminal = new SchematicTerminal(General, this);
            m_terminals.insert(General, terminal);
            break;
        default:;
    }
}

void SchematicDevice::SetName(const QString &name)
{
    m_name = name;
    CreateAnnotation(name);
}

QPixmap SchematicDevice::Image()
{
    if (m_imag)  return m_imag->copy();

    m_imag = new QPixmap(2 * IMAG_LEN, 2 * IMAG_LEN);
    m_imag->fill(Qt::transparent);

    QPainter painter(m_imag);

    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(IMAG_LEN, IMAG_LEN);
    painter.drawPath(path());

    return m_imag->copy();
}

void SchematicDevice::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    // do nothing now.
}

void SchematicDevice::SetAnnotationVisible(bool show)
{
    if (NOT m_annotText)
        return;
    m_annotText->setVisible(show);
}

QRectF SchematicDevice::boundingRect() const
{
    return QRectF(-BRECT_W, -BRECT_W, 2 * BRECT_W, 2 * BRECT_W);
}

QPainterPath SchematicDevice::shape() const
{
    QPainterPath path;

    STerminalTable::const_iterator cit;
    cit = m_terminals.constBegin();
    for (; cit != m_terminals.constEnd(); ++ cit)
        path.addRect(cit.value()->Rect());

    path.addRect(DashRect());

    return path;
}

void SchematicDevice::mousePressEvent(QGraphicsSceneMouseEvent *event)
 {
    QGraphicsPathItem::mousePressEvent(event);
 }

void SchematicDevice::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
    int lineWidth = 2;

    painter->setPen(QPen(m_color, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(path());

    if (m_showTerminal) {
        painter->setBrush(QBrush(m_color));
        painter->setPen(Qt::NoPen);
        STerminalTable::const_iterator cit;
        cit = m_terminals.constBegin();
        for (; cit != m_terminals.constEnd(); ++ cit)
            painter->drawRect(cit.value()->Rect());
    }

    if (isSelected()) {
        painter->setPen(QPen(m_color, 1, Qt::DashLine));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(DashRect());
    }
}

QRectF SchematicDevice::DashRect() const
{
    int halfTerSize = TerminalSize / 2;

    switch (m_deviceType) {
        case RESISTOR:
        case CAPACITOR:
        case INDUCTOR:
        case ISRC:
        case VSRC:
            return QRectF(-12, -BASE_LEN + halfTerSize, 24, (BASE_LEN - halfTerSize) * 2);
        case GND:
            if (m_smallGnd)
                // return QRectF(-3.5, -4, 6, 6);
                return QRectF(-6, -6, 12, 12);
            else
                return QRect(-10, -4, 20, 16);
        default:
            return QRect(-BASE_LEN, -BASE_LEN, BASE_LEN*2, BASE_LEN*2);
    }
}

void SchematicDevice::CreateAnnotation(const QString &text)
{
    // m_annotText = new QGraphicsTextItem();
    m_annotText->setHtml(text);

    m_annotText->setFlag(QGraphicsItem::ItemIsMovable, true);
    m_annotText->setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_annotText->setFont(QFont("Courier 10 Pitch", 10));

    m_annotText->setZValue(-1);
    SetAnnotRelPos();
}

void SchematicDevice::SetAnnotRelPos()
{
    int offsetX = 0, offsetY = 0;
    QRectF dRect, bRect;
    dRect = mapRectToScene(DashRect());
    bRect = m_annotText->boundingRect();

    offsetX = dRect.width()/2 + 1;
    offsetY = -bRect.height()/2;

    m_annotRelPos = QPointF(offsetX, offsetY);
}

void SchematicDevice::AddTerminal(TerminalType type, SchematicTerminal *terminal)
{
    terminal->SetDevice(this);
    m_terminals.insert(type, terminal);
}

void SchematicDevice::SetTerminalRect(TerminalType type, const QRectF &rect)
{
    if (NOT m_terminals.contains(type)) {
        qDebug() << m_name << " doesn't contain " << QString::number(type);
        EXIT;
    }

    SchematicTerminal *terminal = m_terminals[type];
    terminal->SetRect(rect);
}

void SchematicDevice::SetScale(qreal newScale)
{
    setScale(newScale);
    if (m_annotText)
        m_annotText->setScale(newScale);
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
        if (m_annotText) {
            scene()->addItem(m_annotText);
            m_annotText->setPos(mapToScene(m_annotRelPos));
        }
    } else if (change == ItemPositionHasChanged) {
        m_annotText->setPos(mapToScene(m_annotRelPos));
    }

    return QGraphicsItem::itemChange(change, value);
}


void SchematicDevice::SetOrientation(Orientation orien)
{
    if (orien == m_devOrien)  return;
    qreal angle = rotation();
    if (orien == Horizontal) {
        angle -= 90;
    } else {
        angle += 90;
    }

    setRotation(angle);
    m_devOrien = orien;

    /* For annotation text */
    SetAnnotRelPos();

    qreal dx = 0, dy = 0;

    if (orien == Horizontal) {
        if (m_reverse) {
            dx -= m_annotText->boundingRect().height() / 2;
            dy += m_annotText->boundingRect().width();
        } else {
            dx += m_annotText->boundingRect().height() / 2;
        }
    }

    m_annotRelPos.rx() += dx;
    m_annotRelPos.ry() += dy;

    m_annotText->setPos(mapToScene(m_annotRelPos));
}

void SchematicDevice::DrawResistor()
{
    QPainterPath path;

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

    SchematicTerminal *terminal = nullptr;

    int width = TerminalSize;
    int upperCornerX = -halfTerSize;
    int upperCornerY = -BASE_LEN - TerminalSize;
    SetTerminalRect(Positive, QRectF(upperCornerX, upperCornerY, width, width));

    int lowerCornerX = - halfTerSize;
    int lowerCornerY = BASE_LEN;
    SetTerminalRect(Negative, QRectF(lowerCornerX, lowerCornerY, width, width));
}

void SchematicDevice::DrawCapacitor()
{
    QPainterPath path;

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

    SchematicTerminal *terminal = nullptr;
    int width = TerminalSize;
    int upperCornerX = -halfTerSize;
    int upperCornerY = -BASE_LEN - TerminalSize;
    SetTerminalRect(Positive, QRectF(upperCornerX, upperCornerY, width, width));

    int lowerCornerX = -halfTerSize;
    int lowerCornerY = BASE_LEN;
    SetTerminalRect(Negative, QRectF(lowerCornerX, lowerCornerY, width, width));
}

void SchematicDevice::DrawInductor()
{
    QPainterPath path;

    int halfTerSize = TerminalSize / 2;

    path.moveTo(0, - BASE_LEN - halfTerSize);
    path.lineTo(0, -15);
    path.arcTo(-8, -15, 16, 10, 90, -240);
    path.arcTo(-8, -10, 16, 12, 150, -300);
    path.arcTo(-8, -3, 16, 12, 150, -300);
    path.arcTo(-8, 4, 16, 10, 150, -240);

    path.lineTo(0, +BASE_LEN + halfTerSize);

    setPath(path);

    int width = TerminalSize;
    int upperCornerX = -halfTerSize;
    int upperCornerY = -BASE_LEN - TerminalSize;
    SetTerminalRect(Positive, QRectF(upperCornerX, upperCornerY, width, width));

    int lowerCornerX = -halfTerSize;
    int lowerCornerY = BASE_LEN;
    SetTerminalRect(Negative, QRectF(lowerCornerX, lowerCornerY, width, width));
}

void SchematicDevice::DrawIsrc()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

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

    int width = TerminalSize;
    SetTerminalRect(Positive, QRectF(-halfTerSize, -BASE_LEN-TerminalSize, width, width));
    SetTerminalRect(Negative, QRectF(-halfTerSize, BASE_LEN, width, width));
}

void SchematicDevice::DrawVsrc()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
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

    int width = TerminalSize;
    SetTerminalRect(Positive, QRectF(-halfTerSize, -BASE_LEN-TerminalSize, width, width));
    SetTerminalRect(Negative, QRectF(-halfTerSize, BASE_LEN, width, width));
}

QRectF SchematicDevice::GndTerminalRect(bool smallGnd) const
{
    if (smallGnd) {
        /* The terminal-rect is centered at (0, -8) */
        return QRectF(-2, -10, 4, 4);
    } else {
        /* The terminal-rect is centered at (0, -vWire) */
        int vWire = 10;
        return QRectF(-TerminalSize/2, -vWire-TerminalSize/2, TerminalSize, TerminalSize);
    }
}

/*
 *       |
 *     -----
 *      ---
 *       -
 */
void SchematicDevice::DrawGnd()
{
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

    // The terminal-rect is centered at (0, -vWire)
    // QRectF rect(-TerminalSize/2, -vWire-TerminalSize/2, TerminalSize, TerminalSize);
    QRectF rect = GndTerminalRect(/*small gnd*/);
    SetTerminalRect(General, rect);
}

/*
 *
 * black triangle
 *
 */
void SchematicDevice::DrawSmallGnd()
{
    QPainterPath path;

    QVector<QPointF> points;
    points.push_back(QPointF(0, -4));
    points.push_back(QPointF(-3.5, 2));
    points.push_back(QPointF(3.5, 2));

    path.moveTo(0, -8);
    path.lineTo(0, -4);
    path.addPolygon(QPolygonF(points));
    path.closeSubpath();
    setPath(path);

    // The terminal-rect is centered at (0, -8)
    QRectF rect = GndTerminalRect(/*small gnd*/true);
    SetTerminalRect(General, rect);
}

void SchematicDevice::SetGeometricalPos(int col, int row)
{
    Q_ASSERT(col >= 0);
    Q_ASSERT(row >= 0);
    m_geoCol = col;
    m_geoRow = row;
}

void SchematicDevice::SetReverse(bool reverse)
{
    if (m_reverse != reverse) {
        setRotation(180 + rotation());
        // SetAnnotRelPos();
        // ugly implementation
        m_annotRelPos.rx() -= m_annotText->boundingRect().height();
        m_annotRelPos.ry() += (m_annotText->boundingRect().width() / 2 + 6);
        m_annotText->setPos(mapToScene(m_annotRelPos));
    }
    m_reverse = reverse;
}

SchematicTerminal* SchematicDevice::GetTerminal(TerminalType type) const
{
    return m_terminals[type];
}

bool SchematicDevice::GroundCap() const
{
    if (m_deviceType != CAPACITOR)
        return false;

    bool is = false;
    foreach (SchematicTerminal *terminal, m_terminals.values())
        if (terminal->ConnectToGnd()) {
            is = true;
            break;
        }

    return is;
}

bool SchematicDevice::CoupledCap() const
{
    if (m_deviceType != CAPACITOR)
        return false;

    bool is = true;
    foreach (SchematicTerminal *terminal, m_terminals.values())
        if (terminal->ConnectToGnd()) {
            is = false;
            break;
        }

    return is;
}

bool SchematicDevice::TerminalsContain(const QPointF &scenePos) const
{
    QPointF relPos = mapFromScene(scenePos);
    STerminalTable::const_iterator cit;
    cit = m_terminals.constBegin();
    for (; cit != m_terminals.constEnd(); ++ cit) {
        if (cit.value()->Rect().contains(relPos))
            return true;
    }

    return false;
}

void SchematicDevice::RemoveWires(bool deletion)
{
    foreach (SchematicTerminal *ter, m_terminals.values()) {
        foreach (SchematicWire *wire, ter->Wires()) {
            scene()->removeItem(wire);
            wire->StartTerminal()->RemoveWire(wire);
            wire->EndTerminal()->RemoveWire(wire);
            if (deletion) delete wire;
        }
    }
}

void SchematicDevice::UpdateWirePosition()
{
    STerminalTable::const_iterator cit;
    cit = m_terminals.constBegin();
    for (; cit != m_terminals.constEnd(); ++ cit) {
        foreach (SchematicWire *wire, cit.value()->Wires()) {
            if (NOT wire->isSelected())
                wire->UpdatePosition(cit.value());
        }
    }
}

/* BUG */
QPointF SchematicDevice::ScenePosByTerminalScenePos(SchematicTerminal *ter,
                            const QPointF &terScenePos) const
{
    // return terScenePos - (ter->Rect().center()) * scale();
    return terScenePos;
}

void SchematicDevice::SetAsSmallGnd(bool smallGnd)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << smallGnd << endl;
#endif

    if (m_smallGnd == smallGnd)
        return;

    m_smallGnd = smallGnd;

    if (m_smallGnd) {
        DrawSmallGnd();
    } else {
        DrawGnd();
    }
    m_isDevice = false;
}

void SchematicDevice::ClearConnectors()
{
    foreach (SConnector *scd, m_connectors)
        delete scd;
    m_connectors.clear();
}

void SchematicDevice::AddConnector(SConnector *desp)
{
    m_connectors.push_back(desp);
}

SchematicTerminal* SchematicDevice::ConnectTerminal() const
{
    SchematicTerminal *terminal = nullptr;

    foreach (SConnector *scd, m_connectors) {
        terminal = scd->connectTerminal;
        break;
    }

    Q_ASSERT(terminal);

    return terminal;
}

SchematicTerminal* SchematicDevice::ConnectTerminal(TerminalType type) const
{
    SchematicTerminal *terminal = nullptr;

    foreach (SConnector *scd, m_connectors)
        if (scd->thisTerminal->GetTerminalType() == type) {
            terminal = scd->connectTerminal;
            break;
        }

    Q_ASSERT(terminal);

    return terminal;
}

/* Print and Plot */
void SchematicDevice::Print() const
{
    printf("-------------------------\n");
    QString tmp;
    tmp += (m_name + " " + "type(" + QString::number(m_deviceType) + "), ");
    tmp += ("id(" + QString::number(m_id) + "), ");
    tmp += ("geoCol(" + QString::number(m_geoCol) + "), ");
    tmp += ("geoRow(" + QString::number(m_geoRow) + "), ");
    tmp += ((m_devOrien == Horizontal)? "orien(H), " : "orien(V), ");
    tmp += ((m_reverse) ? "reverse(yes)" : "reverse(no)");
    qInfo() << tmp;
    qInfo() << "Terminals";
    foreach (SchematicTerminal *ter, m_terminals.values())
        qInfo() << ter->PrintString();
    printf("-------------------------\n");
}
