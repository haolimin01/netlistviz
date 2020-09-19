#include "MainWindow.h"
#include <QtWidgets>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <QRectF>
#include <QList>
#include "Schematic/SchematicScene.h"
#include "Schematic/SchematicView.h"
#include "Schematic/SchematicTextItem.h"
#include "Schematic/SchematicWire.h"
#include "Define/Define.h"
#include "Schematic/NetlistDialog.h"
#include "Parser/MyParser.h"
#include "ASG/ASG.h"
#include "Schematic/ASGDialog.h"

const int DEV_ICON_SIZE = 30;


MainWindow::MainWindow()
{
    InitVariables();

    CreateActions();

    /* Define the main window corners for placing the DockWidget areas */
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::TopDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);
    CreateDeviceToolBox();

    CreateMenus();
    CreateToolBars();

    CreateSchematicScene();
    CreateCenterWidget();
    setWindowTitle(tr("netlistviz"));
    setUnifiedTitleAndToolBarOnMac(true);

    connect(this, &MainWindow::NetlistChanged, this, &MainWindow::NetlistChangedSlot);
}

MainWindow::~MainWindow()
{
    m_scene->clear();
    // if (m_data)  m_data->Clear();
    if (m_asg)  delete m_asg;
    if (m_netlistDialog)  delete m_netlistDialog;
    if (m_asgDialog)  delete m_asgDialog;
}

void MainWindow::InitVariables()
{
    m_curNetlistPath = ".";
    m_curNetlistFile = "";
    m_curSchematicPath = ".";
    m_curSchematicFile = "";
    m_ckt = nullptr;
    m_asg = nullptr;
    m_deviceBeingAdded = nullptr;

    m_netlistDialog = new NetlistDialog();

    m_asgPropertySelected = false;
    m_asgDialog = nullptr;
}

void MainWindow::CreateSchematicScene()
{
    m_scene = new SchematicScene(m_editMenu, this);

    m_scene->setSceneRect(QRectF(0, 0, Scene_W, Scene_H));
    m_scene->setFocus(Qt::MouseFocusReason);

    connect(m_scene, &SchematicScene::TextInserted,
            this, &MainWindow::TextInserted);
    connect(m_scene, &SchematicScene::DeviceInserted,
            this, &MainWindow::DeviceInserted);
    connect(m_scene, &SchematicScene::changed,
            this, &MainWindow::UpdateWindowTitle);
}

void MainWindow::CreateCenterWidget()
{
    QHBoxLayout *layout = new QHBoxLayout;
    m_view = new SchematicView(m_scene);
    m_view->setRenderHints(QPainter::Antialiasing
                        | QPainter::NonCosmeticDefaultPen
                        | QPainter::TextAntialiasing);
    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    m_view->setMouseTracking(true);
    layout->addWidget(m_view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
}

void MainWindow::DeviceBtnGroupClicked(int id)
{
    const QList<QAbstractButton *> buttons = m_deviceBtnGroup->buttons();
    for (QAbstractButton *button : buttons) {
        if (m_deviceBtnGroup->button(id) != button) {
            button->setChecked(false);
        }
    }
    m_scene->SetMode(SchematicScene::InsertDeviceMode);
    m_scene->SetDeviceType(DeviceType(id));

    if (m_deviceBeingAdded) {
        delete m_deviceBeingAdded;
        m_deviceBeingAdded = nullptr;
    }

    m_deviceBeingAdded = new SchematicDevice(DeviceType(id), m_editMenu);
    QPixmap image(m_deviceBeingAdded->Image());
    m_view->setCursor(QCursor(image.scaled(30, 30)));
}

void MainWindow::closeEvent(QCloseEvent *closeEvent)
{
    if (m_asg) {
        delete m_asg;
        m_asg = nullptr;
    }
    QMainWindow::closeEvent(closeEvent);
}

void MainWindow::DeleteItem()
{
#if 0
    QList<QGraphicsItem *> selectedItems = m_scene->selectedItems();
    foreach (QGraphicsItem *item, qAsConst(selectedItems)) {
        SchematicDevice *device = nullptr;
        TerminalType terminal;
        if (item->type() == SchematicWire::Type) {
            m_scene->removeItem(item);
            SchematicWire *wire = qgraphicsitem_cast<SchematicWire *>(item);
            device = wire->StartDevice();
            terminal = wire->StartTerminal();
            device->RemoveWire(wire, terminal);
            device = wire->EndDevice();
            terminal = wire->EndTerminal();
            device->RemoveWire(wire, terminal);
            delete item;
        } else if (item->type() == SchematicTextItem::Type) {
            m_scene->removeItem(item);
            delete item;
        }
    }

    selectedItems = m_scene->selectedItems();
    foreach (QGraphicsItem *item, qAsConst(selectedItems)) {
        if (item->type() == SchematicDevice::Type) {
            SchematicDevice *device = qgraphicsitem_cast<SchematicDevice *>(item);
            device->RemoveWires(true);
            m_scene->removeItem(item);
            delete item;
        }
    }
#endif
}

/* BUG */
void MainWindow::PointerBtnGroupClicked(int id)
{
    m_scene->SetMode(SchematicScene::Mode(m_pointerBtnGroup->checkedId()));

    if (id == int(SchematicScene::BaseMode)) {

        m_pointerBtnGroup->button(int(SchematicScene::BaseMode))->setChecked(true);
        m_pointerBtnGroup->button(int(SchematicScene::InsertTextMode))->setChecked(false);

    } else if (id == int(SchematicScene::InsertTextMode)) {

        m_pointerBtnGroup->button(int(SchematicScene::BaseMode))->setChecked(false);
        m_pointerBtnGroup->button(int(SchematicScene::InsertTextMode))->setChecked(true);
    }
}

/* BUG */
void MainWindow::BringToFront()
{
    if (m_scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = m_scene->selectedItems().first();

    qreal zValue = 0;
    selectedItem->setZValue(zValue);
}

/* BUG */
void MainWindow::SendToBack()
{
    if (m_scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = m_scene->selectedItems().first();

    qreal zValue = 0;
    selectedItem->setZValue(zValue);
}

void MainWindow::TextInserted(QGraphicsTextItem *)
{
    m_pointerBtnGroup->button(int(SchematicScene::BaseMode))->setChecked(true);
    m_pointerBtnGroup->button(int(SchematicScene::InsertTextMode))->setChecked(false);
    m_scene->SetMode(SchematicScene::BaseMode);
}

void MainWindow::DeviceInserted(SchematicDevice *device)
{
    m_pointerBtnGroup->button(int(SchematicScene::BaseMode))->setChecked(true);
    m_pointerBtnGroup->button(int(SchematicScene::InsertTextMode))->setChecked(false);
    m_scene->SetMode(SchematicScene::BaseMode);

    m_deviceBtnGroup->button((int)(device->GetDeviceType()))->setChecked(false);
    m_view->setCursor(Qt::ArrowCursor);
}

void MainWindow::CurrentFontChanged(const QFont &)
{
    HandleFontChange();
}

void MainWindow::FontSizeChanged(const QString &)
{
    HandleFontChange();
}

void MainWindow::ZoomActionToggled(bool enable)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
    m_view->EnableScaleByWheel(enable);
}

void MainWindow::ScaleToOriginTriggered()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    m_view->setResizeAnchor(QGraphicsView::NoAnchor);
    qreal currHScale = m_view->matrix().m11();
    qreal currVScale = m_view->matrix().m22();
    m_view->scale(1 / currHScale, 1 / currVScale);
}

void MainWindow::MoveToCenterTriggered()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
    m_view->centerOn(m_scene->Center());
}

void MainWindow::TextColorChanged()
{
    m_textAction = qobject_cast<QAction *>(sender());
    m_fontColorToolButton->setIcon(CreateColorToolButtonIcon(
        ":/images/textpointer.png",
        qvariant_cast<QColor>(m_textAction->data())));
    TextButtonTriggered();
}

// void MainWindow::WireColorChanged()
// {
// 
// }

void MainWindow::TextButtonTriggered()
{
    m_scene->SetTextColor(qvariant_cast<QColor>(m_textAction->data()));
}

void MainWindow::HandleFontChange()
{
    QFont font = m_fontCombo->currentFont();
    font.setPointSize(m_fontSizeCombo->currentText().toInt());
    font.setWeight(m_boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(m_italicAction->isChecked());
    font.setUnderline(m_underlineAction->isChecked());

    m_scene->SetFont(font);
}

void MainWindow::ItemSelected(QGraphicsItem *item)
{
    SchematicTextItem *textItem =
        qgraphicsitem_cast<SchematicTextItem *>(item);

    QFont font = textItem->font();
    m_fontCombo->setCurrentFont(font);
    m_fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    m_boldAction->setChecked(font.weight() == QFont::Bold);
    m_italicAction->setChecked(font.italic());
    m_underlineAction->setChecked(font.underline());
}

void MainWindow::About()
{
    QMessageBox::about(this, tr("About netlistviz"),
                       tr("The <b>netlistviz</b> shows "
                          "SPICE netlist"));
}

void MainWindow::CreateDeviceToolBox()
{
    m_deviceBtnGroup = new QButtonGroup(this);
    m_deviceBtnGroup->setExclusive(false);
    connect(m_deviceBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &MainWindow::DeviceBtnGroupClicked);

    QGridLayout *layout = new QGridLayout;

    /* Add device(s) */
    layout->addWidget(CreateCellWidget(tr("Resistor"), RESISTOR), 0, 0);
    layout->addWidget(CreateCellWidget(tr("Capacitor"), CAPACITOR), 0, 1);
    layout->addWidget(CreateCellWidget(tr("Inductor"), INDUCTOR), 1, 0);
    layout->addWidget(CreateCellWidget(tr("ISource"), ISRC), 1, 1);
    layout->addWidget(CreateCellWidget(tr("VSource"), VSRC), 2, 0);
    // layout->addWidget(CreateCellWidget(tr("Dot"), SchematicDevice::Dot), 2, 1);
    layout->addWidget(CreateCellWidget(tr("GND"), GND), 2, 1);

    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

    m_deviceToolBox = new QToolBox;
    m_deviceToolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    m_deviceToolBox->setMinimumWidth(itemWidget->sizeHint().width());
    m_deviceToolBox->addItem(itemWidget, tr(""));

    /* Create device panel */
    m_devicePanelDockWidget = new QDockWidget(tr("Devices"));
    m_devicePanelDockWidget->setWidget(m_deviceToolBox);
    m_devicePanelDockWidget->setGeometry(10, 30, 100, 70);
    m_devicePanelDockWidget->hide();
    addDockWidget(Qt::LeftDockWidgetArea, m_devicePanelDockWidget);
}

void MainWindow::CreateActions()
{
    m_toFrontAction = new QAction(QIcon(":/images/bringtofront.png"),
                                tr("Bring to &Front"), this);
    m_toFrontAction->setShortcut(tr("Ctrl+F"));
    m_toFrontAction->setStatusTip(tr("Bring item to front"));
    connect(m_toFrontAction, &QAction::triggered, this, &MainWindow::BringToFront);

    m_sendBackAction = new QAction(QIcon(":/images/sendtoback.png"), tr("Send to &Back"), this);
    m_sendBackAction->setShortcut(tr("Ctrl+T"));
    m_sendBackAction->setStatusTip(tr("Send item to back"));
    connect(m_sendBackAction, &QAction::triggered, this, &MainWindow::SendToBack);

    m_deleteAction = new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
    m_deleteAction->setShortcut(tr("Delete"));
    m_deleteAction->setStatusTip(tr("Delete item from Scene"));
    connect(m_deleteAction, &QAction::triggered, this, &MainWindow::DeleteItem);

    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcuts(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("Quit netlisviz"));
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    m_boldAction = new QAction(tr("Bold"), this);
    m_boldAction->setCheckable(true);
    QPixmap pixmap(":/images/bold.png");
    m_boldAction->setIcon(QIcon(pixmap));
    m_boldAction->setShortcut(tr("Ctrl+B"));
    connect(m_boldAction, &QAction::triggered, this, &MainWindow::HandleFontChange);

    m_italicAction = new QAction(QIcon(":/images/italic.png"), tr("Italic"), this);
    m_italicAction->setCheckable(true);
    m_italicAction->setShortcut(tr("Ctrl+I"));
    connect(m_italicAction, &QAction::triggered, this, &MainWindow::HandleFontChange);

    m_underlineAction = new QAction(QIcon(":/images/underline.png"), tr("Underline"), this);
    m_underlineAction->setCheckable(true);
    m_underlineAction->setShortcut(tr("Ctrl+U"));
    connect(m_underlineAction, &QAction::triggered, this, &MainWindow::HandleFontChange);

    m_aboutAction = new QAction(tr("A&bout"), this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::About);

    m_openNetlistAction = new QAction(QIcon(":/images/open_netlist.png"), tr("Open N&etlist"), this);
    m_openNetlistAction->setShortcut(tr("Ctrl+N"));
    connect(m_openNetlistAction, &QAction::triggered, this, &MainWindow::OpenNetlist);

    m_saveSchematicFileAction = new QAction(QIcon(":/images/save_schematic.png"), tr("S&ave Schematic"), this);
    m_saveSchematicFileAction->setShortcut(QKeySequence::Save);
    connect(m_saveSchematicFileAction, &QAction::triggered, this, &MainWindow::SaveSchematicFile);

    m_saveAsSchematicFileAction = new QAction(QIcon(":/images/saveas_schematic.png"), tr("SaveA&s Schematic"), this);
    connect(m_saveAsSchematicFileAction, &QAction::triggered, this, &MainWindow::SaveAsSchematicFile);

    m_openSchematicFileAction = new QAction(QIcon(":/images/open_schematic.png"), tr("Open S&chematic"), this);
    m_openSchematicFileAction->setShortcut(QKeySequence::Open);
    connect(m_openSchematicFileAction, &QAction::triggered, this, &MainWindow::OpenSchematic);

    m_scrollPointerAction = new QAction(QIcon(":/images/scroll.png"), tr("Scroll Pointer"), this);
    m_scrollPointerAction->setCheckable(true);
    m_scrollPointerAction->setChecked(false);
    connect(m_scrollPointerAction, &QAction::toggled, this, &MainWindow::ScrollActionToggled);

    m_zoomAction = new QAction(QIcon(":/images/zoom.png"), tr("Zoom by Wheel"), this);
    m_zoomAction->setCheckable(true);
    m_zoomAction->setChecked(false);
    connect(m_zoomAction, &QAction::toggled, this, &MainWindow::ZoomActionToggled);

    m_moveToCenterAction = new QAction(QIcon(":/images/move_center.png"), tr("Move to center"), this);
    m_moveToCenterAction->setShortcut(tr("C"));
    connect(m_moveToCenterAction, &QAction::triggered, this,
        &MainWindow::MoveToCenterTriggered);

    m_scaleToOriginAction = new QAction(QIcon(":/images/scale_to_origin.png"), tr("Scale to origin"), this);
    m_scaleToOriginAction->setShortcut(tr("O"));
    connect(m_scaleToOriginAction, &QAction::triggered, this, &MainWindow::ScaleToOriginTriggered);

    m_showTerminalAction = new QAction(QIcon(":/images/show_terminal.png"), tr("Show Terminal"), this);
    m_showTerminalAction->setCheckable(true);
    m_showTerminalAction->setChecked(false);
    connect(m_showTerminalAction, &QAction::toggled, this, &MainWindow::ShowItemTerminalToggled);

    m_showBranchAction = new QAction(QIcon(":/images/show_branch.png"), tr("Show Branch"), this);
    m_showBranchAction->setCheckable(true);
    m_showBranchAction->setChecked(false);
    connect(m_showBranchAction, &QAction::toggled, this, &MainWindow::ShowBranchToggled);

    m_showGridAction = new QAction(QIcon(":/images/grid.png"), tr("Show Grid"), this);
    m_showGridAction->setCheckable(true);
    m_showGridAction->setChecked(true);
    connect(m_showGridAction, &QAction::toggled, this, &MainWindow::ShowGridToggled);

    m_parseNetlistAction = new QAction(QIcon(":/images/parse_netlist.png"), tr("Parse Netlist"), this);
    m_parseNetlistAction->setEnabled(false);
    connect(m_parseNetlistAction, &QAction::triggered, this, &MainWindow::ParseNetlist);

    m_asgPropertyAction = new QAction(QIcon(":/images/asg_property.png"), tr("ASG Property"), this);
    m_asgPropertyAction->setEnabled(false);
    connect(m_asgPropertyAction, &QAction::triggered, this, &MainWindow::ASGPropertyTriggered);

    m_logPlaceAction = new QAction(QIcon(":/images/log_place.png"), tr("Logical Placement"), this);
    m_logPlaceAction->setEnabled(false);
    connect(m_logPlaceAction, &QAction::triggered, this, &MainWindow::LogicalPlacement);
    
    m_logRouteAction = new QAction(QIcon(":/images/log_route.png"), tr("Logical Routing"), this);
    m_logRouteAction->setEnabled(false);
    connect(m_logRouteAction, &QAction::triggered, this, &MainWindow::LogicalRouting);

    m_geoPlaceAction = new QAction(QIcon(":/images/geo_place.png"), tr("Geometrical Placement"), this);
    m_geoPlaceAction->setEnabled(false);
    connect(m_geoPlaceAction, &QAction::triggered, this, &MainWindow::GeometricalPlacement);

    m_geoRouteAction = new QAction(QIcon(":/images/geo_route.png"), tr("Geometrical Routing"), this);
    m_geoRouteAction->setEnabled(false);
    connect(m_geoRouteAction, &QAction::triggered, this, &MainWindow::GeometricalRouting);
}

void MainWindow::CreateMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_openNetlistAction);
    m_fileMenu->addAction(m_openSchematicFileAction);
    m_fileMenu->addAction(m_saveSchematicFileAction);
    m_fileMenu->addAction(m_saveAsSchematicFileAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    m_editMenu = menuBar()->addMenu(tr("&Edit"));
    m_editMenu->addAction(m_deleteAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_toFrontAction);
    m_editMenu->addAction(m_sendBackAction);

    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_viewMenu->addAction(m_devicePanelDockWidget->toggleViewAction());
    m_viewMenu->addAction(m_showTerminalAction);
    m_viewMenu->addAction(m_showBranchAction);
    m_viewMenu->addAction(m_showGridAction);
    m_viewMenu->addAction(m_zoomAction);
    m_viewMenu->addAction(m_moveToCenterAction);
    m_viewMenu->addAction(m_scaleToOriginAction);

    m_asgMenu = menuBar()->addMenu(tr("&ASG"));
    m_asgMenu->addAction(m_parseNetlistAction);
    m_asgMenu->addAction(m_asgPropertyAction);
    m_asgMenu->addAction(m_logPlaceAction);
    m_asgMenu->addAction(m_logRouteAction);
    m_asgMenu->addAction(m_geoPlaceAction);
    m_asgMenu->addAction(m_geoRouteAction);

    m_aboutMenu = menuBar()->addMenu(tr("&Help"));
    m_aboutMenu->addAction(m_aboutAction);
}

void MainWindow::CreateToolBars()
{
    m_fileToolBar = addToolBar(tr("File"));
    m_fileToolBar->addAction(m_openNetlistAction);
    // m_fileToolBar->addAction(m_openSchematicFileAction);
    // m_fileToolBar->addAction(m_saveSchematicFileAction);
    // m_fileToolBar->addAction(m_saveAsSchematicFileAction);

    m_editToolBar = addToolBar(tr("Edit"));
    m_editToolBar->addAction(m_deleteAction);
    m_editToolBar->addAction(m_toFrontAction);
    m_editToolBar->addAction(m_sendBackAction);
    m_editToolBar->addAction(m_showTerminalAction);
    m_editToolBar->addAction(m_showBranchAction);
    m_editToolBar->addAction(m_showGridAction);
    m_editToolBar->addAction(m_zoomAction);
    m_editToolBar->addAction(m_moveToCenterAction);
    m_editToolBar->addAction(m_scaleToOriginAction);

    m_fontCombo = new QFontComboBox();
    connect(m_fontCombo, &QFontComboBox::currentFontChanged,
            this, &MainWindow::CurrentFontChanged);

    m_fontSizeCombo = new QComboBox;
    m_fontSizeCombo->setEditable(true);
    for (int i = 8; i < 50; i = i + 2)
        m_fontSizeCombo->addItem(QString().setNum(i));
    QIntValidator *validator = new QIntValidator(2, 64, this);
    m_fontSizeCombo->setValidator(validator);
    connect(m_fontSizeCombo, &QComboBox::currentTextChanged,
            this, &MainWindow::FontSizeChanged);

    m_fontColorToolButton = new QToolButton;
    m_fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_fontColorToolButton->setMenu(CreateColorMenu(SLOT(TextColorChanged()), Qt::black));
    m_textAction = m_fontColorToolButton->menu()->defaultAction();
    m_fontColorToolButton->setIcon(CreateColorToolButtonIcon(":/images/textpointer.png", Qt::black));
    m_fontColorToolButton->setAutoFillBackground(true);
    connect(m_fontColorToolButton, &QAbstractButton::clicked,
            this, &MainWindow::TextButtonTriggered);

    m_textToolBar = addToolBar(tr("Font"));
    // m_textToolBar->addWidget(m_fontCombo);
    m_textToolBar->addWidget(m_fontSizeCombo);
    m_textToolBar->addAction(m_boldAction);
    m_textToolBar->addAction(m_italicAction);
    m_textToolBar->addAction(m_underlineAction);

    m_colorToolBar = addToolBar(tr("Color"));
    m_colorToolBar->addWidget(m_fontColorToolButton);

    QToolButton *baseModePointerButton = new QToolButton;
    baseModePointerButton->setCheckable(true);
    baseModePointerButton->setChecked(true);
    baseModePointerButton->setIcon(QIcon(":/images/pointer.png"));

    QToolButton *insertTextPointerButton = new QToolButton;
    insertTextPointerButton->setCheckable(true);
    insertTextPointerButton->setIcon(QIcon(":/images/textpointer.png"));

    m_pointerBtnGroup = new QButtonGroup(this);
    m_pointerBtnGroup->setExclusive(true);
    m_pointerBtnGroup->addButton(baseModePointerButton, int(SchematicScene::BaseMode));
    m_pointerBtnGroup->addButton(insertTextPointerButton, int(SchematicScene::InsertTextMode));
    connect(m_pointerBtnGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(PointerBtnGroupClicked(int)));

    m_pointerToolBar = addToolBar(tr("Pointer type"));
    m_pointerToolBar->addWidget(baseModePointerButton);
    m_pointerToolBar->addAction(m_scrollPointerAction);
    m_pointerToolBar->addWidget(insertTextPointerButton);

    m_asgToolBar = addToolBar(tr("ASG"));
    m_asgToolBar->addAction(m_parseNetlistAction);
    m_asgToolBar->addAction(m_asgPropertyAction);
    m_asgToolBar->addAction(m_logPlaceAction);
    m_asgToolBar->addAction(m_logRouteAction);
    m_asgToolBar->addAction(m_geoPlaceAction);
    m_asgToolBar->addAction(m_geoRouteAction);
}

QWidget *MainWindow::CreateCellWidget(const QString &text, DeviceType type)
{

    SchematicDevice device(type, nullptr);
    QIcon icon(device.Image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(DEV_ICON_SIZE, DEV_ICON_SIZE));
    button->setCheckable(true);
    m_deviceBtnGroup->addButton(button, type);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

QMenu *MainWindow::CreateColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
          << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i)
    {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(CreateColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor)
            colorMenu->setDefaultAction(action);
    }
    return colorMenu;
}

QIcon MainWindow::CreateColorToolButtonIcon(const QString &imageFile, QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    // Draw icon centred horizontally on button.
    QRect target(4, 0, 42, 43);
    QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}

QIcon MainWindow::CreateColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}

void MainWindow::ShowItemTerminalToggled(bool show)
{
    m_scene->SetShowTerminal(show);
}

void MainWindow::ShowBranchToggled(bool show)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    // m_scene->SetShowBranchFlag(show);
}

void MainWindow::ShowGridToggled(bool show)
{
    m_scene->SetShowBackground(show);
    m_scene->update();
}

void MainWindow::OpenNetlist()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    QString fileName;
    QString fileFilters;
    fileFilters = tr("Netlist files (*.sp)\n" "All files (*)");

    fileName = QFileDialog::getOpenFileName(this, tr("Open Netlist..."),
                            m_curNetlistPath, fileFilters);
    
    if (fileName.isEmpty())
        return;

    m_curNetlistPath = QFileInfo(fileName).path();
    m_curNetlistFile = fileName;

    emit NetlistChanged();

    ShowNetlistFile(m_curNetlistFile);
}

void MainWindow::NetlistChangedSlot()
{
    m_parseNetlistAction->setEnabled(true);
    m_asgPropertyAction->setEnabled(true);
    m_logPlaceAction->setEnabled(true);
    m_logRouteAction->setEnabled(true);
    m_geoPlaceAction->setEnabled(true);
    m_geoRouteAction->setEnabled(true);

    m_asgPropertySelected = false;
}

void MainWindow::OpenSchematic()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif 

#if 0
    QString fileName;
    QString fileFilters;
    fileFilters = tr("Schematic files (*.sch)\n" "All files(*)");

    fileName = QFileDialog::getOpenFileName(this, tr("OPen Schematic..."),
                            m_curSchematicPath, fileFilters);
    
    if (fileName.isEmpty())
        return;

    m_curSchematicPath = QFileInfo(fileName).path();
    m_curSchematicFile = fileName;

    QFile file(m_curSchematicFile);
    bool ok = file.open(QIODevice::ReadOnly);
    if (NOT ok) {
        QString errMsg = "Open " + m_curSchematicFile + " failed.";
        ShowCriticalMsg(errMsg);
        return;
    }
    QTextStream stream(&file);
    m_scene->LoadSchematicFromStream(stream);
    file.close();

    QString dispName = QFileInfo(m_curSchematicFile).fileName();
    setWindowTitle(dispName);
#endif
}

void MainWindow::UpdateWindowTitle(const QList<QRectF> &)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
    if (m_curSchematicFile.isEmpty()) {
        setWindowTitle(tr("netlistviz"));
        return;
    }

    QString dispName = "*";
    dispName += QFileInfo(m_curSchematicFile).fileName();
    setWindowTitle(dispName);
}

void MainWindow::SaveSchematicFile() 
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
#if 0
    if (m_curSchematicFile.isEmpty())
        return SaveAsSchematicFile();

    QFile file(m_curSchematicFile);
    bool ok = file.open(QIODevice::WriteOnly);
    if (NOT ok) {
        QString errMsg = "Open " + m_curSchematicFile + " failed.";
        ShowCriticalMsg(errMsg);
        return;
    }
    QTextStream stream(&file);
    m_scene->WriteSchematicToStream(stream);
    file.close();

    QString dispName = QFileInfo(m_curSchematicFile).fileName();
    setWindowTitle(dispName);

#ifdef DEBUG
    qInfo() << "Save to " << m_curSchematicFile << endl;
#endif
#endif
}

void MainWindow::SaveAsSchematicFile()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
#if 0
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Schematic As"),
                                    m_curSchematicPath, tr("Schematic files (*.sch)"));
    
    /* Cancel clicked */
    if (fileName.isEmpty()) {
        return;
    }

    m_curSchematicPath = QFileInfo(fileName).path();
    m_curSchematicFile = fileName;

    QFile file(fileName);
    bool ok = file.open(QIODevice::WriteOnly);
    if (NOT ok) {
        QString errMsg = "Open " + m_curSchematicFile + " failed.";
        ShowCriticalMsg(errMsg);
        return;
    }

    QTextStream stream(&file);
    m_scene->WriteSchematicToStream(stream);
    file.close();

    QString dispName = QFileInfo(m_curSchematicFile).fileName();
    setWindowTitle(dispName);
#endif
}

void MainWindow::ShowNetlistFile(const QString &netlist)
{
    m_netlistDialog->SetNetlistFile(netlist);
    m_netlistDialog->show();
}

void MainWindow::ParseNetlist()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    if (m_ckt) delete m_ckt;
    /* delete all devices, nodes, terminals */
    m_scene->clear();

    MyParser parser;
    m_ckt = new CircuitGraph();
    int error = parser.ParseNetlist(m_curNetlistFile.toStdString(), m_ckt);
    if (error) {
        delete m_ckt;
        ShowCriticalMsg(tr("Parse Netlist failed."));
        return;
    }

#ifdef DEBUG
    m_ckt->PrintCircuit();
#endif
    ShowInfoMsg(tr("Parse Netlist successfully."));
}

void MainWindow::ShowCriticalMsg(const QString &msg)
{
    QMessageBox::critical(this, tr("Critical Message"), msg);
}

void MainWindow::ShowInfoMsg(const QString &msg)
{
    QMessageBox::information(this, tr("Information"), msg);
}

void MainWindow::ScrollActionToggled(bool checked)
{
    if (checked) {
        m_view->setDragMode(QGraphicsView::ScrollHandDrag);
        m_pointerBtnGroup->button(int(SchematicScene::BaseMode))->setChecked(false);
    } else {
        m_view->setDragMode(QGraphicsView::RubberBandDrag);
        m_pointerBtnGroup->button(int(SchematicScene::BaseMode))->setChecked(true);
    }

    m_pointerBtnGroup->button(int(SchematicScene::InsertTextMode))->setChecked(false);
}

void MainWindow::ASGPropertyTriggered()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    Q_ASSERT(m_ckt);
    if (m_asgDialog) delete m_asgDialog;
    m_asgDialog = new ASGDialog();

    m_asgDialog->SetCircuitGraph(m_ckt);
    m_asgDialog->show();
    m_asgPropertySelected = true;

    if (m_asg) delete m_asg;
    m_asg = new ASG(m_ckt);
}

void MainWindow::LogicalPlacement()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    if (m_ckt->FirstLevelDeviceListSize() < 1) {
        ShowCriticalMsg(tr("Please select first level device(s)"));
        m_asgPropertySelected = false;
        return;
    }

    if (NOT m_asgPropertySelected) {
        ShowCriticalMsg(tr("Please Select ASG Properties firstly!"));
        return;
    }

    int error = m_asg->LogicalPlacement();
    if (error) {
        ShowCriticalMsg(tr("[ERROR ASG] Logicl Placement failed."));
    }
}

void MainWindow::LogicalRouting()
{
    int error = m_asg->LogicalRouting();
    if (error) {
        ShowCriticalMsg(tr("[ERROR ASG] Logical Routing failed."));
    }
}

void MainWindow::GeometricalPlacement()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    int error = m_asg->GeometricalPlacement(m_scene);
    if (error) {
        ShowCriticalMsg(tr("[ERROR ASG] Geometrical Placement failed."));
    }

    m_view->centerOn(m_scene->Center());
}

void MainWindow::GeometricalRouting()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    int error = m_asg->GeometricalRouting();
    if (error) {
        ShowCriticalMsg(tr("[ERROR ASG] Geometrical Routing failed."));
    }
}
