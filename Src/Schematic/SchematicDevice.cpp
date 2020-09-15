#include "SchematicDevice.h"
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QGraphicsScene>
#include "Circuit/Device.h"


static const int TerminalSize = 8;
static const int IMAG_LEN = 25;
static const int BASE_LEN = 20;
static const int BRECT_W = 30;


SchematicDevice::SchematicDevice(Device *dev, QMenu *contextMenu,
    QTransform transform, QGraphicsItem *parent)
{
    Q_ASSERT(dev);
    m_cktdev = dev;
    m_contextMenu = contextMenu;
    m_deviceType = dev->GetDeviceType();

    setTransform(transform);
    Initialize();
}

SchematicDevice::SchematicDevice(DeviceType type, QMenu *contextMenu,
    QTransform transform, QGraphicsItem *parent)
{

}

SchematicDevice::~SchematicDevice()
{
    /* delete Device here */
    if (m_imag) delete m_imag;
    if (m_annotText) delete m_annotText;
    if (m_cktdev) delete m_cktdev;
}

void SchematicDevice::Initialize()
{
    m_color = Qt::black;
    setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    
    switch (m_deviceType) {
        case RESISTOR:
            m_devOrien = Vertical;
            DrawResistor();
            m_isDevice = true;
            break;
        case CAPACITOR:
            m_devOrien = Vertical;
            DrawCapacitor();
            m_isDevice = true;
            break;
        case INDUCTOR:
            m_devOrien = Vertical;
            DrawInductor();
            m_isDevice = true;
            break;
        case ISRC:
            m_devOrien = Vertical;
            DrawIsrc();
            m_isDevice = true;
            break;
        case VSRC:
            m_devOrien = Vertical;
            DrawVsrc();
            m_isDevice = true;
            break;
        case GND:
            m_devOrien = Vertical;
            DrawGnd();
            m_isDevice = false;
            break;
        default:;
    }

    m_imag = nullptr;
    m_showNodeFlag = false;

    CreateAnnotation(m_cktdev->Name());

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    setZValue(0);
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

QRectF SchematicDevice::boundingRect() const
{
    return QRectF(-BRECT_W, -BRECT_W, 2 * BRECT_W, 2 * BRECT_W);
}

QPainterPath SchematicDevice::shape() const
{
    QPainterPath path;

    TerminalRectTable::const_iterator cit;
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

void SchematicDevice::mousePressEvent(QGraphicsSceneMouseEvent *event)
 {
    QGraphicsPathItem::mousePressEvent(event);
 }

void SchematicDevice::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *)
{

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
            return QRect(-10, -4, 20, 16);
        default:
            return QRect(-BASE_LEN, -BASE_LEN, BASE_LEN*2, BASE_LEN*2);
    }
}

void SchematicDevice::CreateAnnotation(const QString &text)
{
    m_annotText = new QGraphicsTextItem();
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

    // int width = TerminalSize;
    // int upperCornerX = -halfTerSize;
    // int upperCornerY = -BASE_LEN - TerminalSize;
    // m_terRects.insert(Positive, QRectF(upperCornerX, upperCornerY, width, width));

    // int lowerCornerX = - halfTerSize;
    // int lowerCornerY = BASE_LEN;
    // m_terRects.insert(Negative, QRectF(lowerCornerX, lowerCornerY, width, width));
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

    // int width = TerminalSize;
    // int upperCornerX = -halfTerSize;
    // int upperCornerY = -BASE_LEN - TerminalSize;
    // m_terRects.insert(Positive, QRectF(upperCornerX, upperCornerY, width, width));

    // int lowerCornerX = -halfTerSize;
    // int lowerCornerY = BASE_LEN;
    // m_terRects.insert(Negative, QRectF(lowerCornerX, lowerCornerY, width, width));
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

    // int width = TerminalSize;
    // int upperCornerX = -halfTerSize;
    // int upperCornerY = -BASE_LEN - TerminalSize;
    // m_terRects.insert(Positive, QRectF(upperCornerX, upperCornerY, width, width));

    // int lowerCornerX = -halfTerSize;
    // int lowerCornerY = BASE_LEN;
    // m_terRects.insert(Negative, QRectF(lowerCornerX, lowerCornerY, width, width));
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

    // int width = TerminalSize;
    // m_terRects.insert(Positive, QRectF(-halfTerSize, -BASE_LEN-TerminalSize, width, width));
    // m_terRects.insert(Negative, QRectF(-halfTerSize, BASE_LEN, width, width));
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

    // int width = TerminalSize;
    // m_terRects.insert(Positive, QRectF(-halfTerSize, -BASE_LEN-TerminalSize, width, width));
    // m_terRects.insert(Negative, QRectF(-halfTerSize, BASE_LEN, width, width));
}

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
    // m_terRects.insert(General, rect);
}