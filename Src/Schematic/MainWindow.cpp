#include "MainWindow.h"
#include <QtWidgets>
#include <QDebug>
#include "SchematicScene.h"
#include "SchematicTextItem.h"
#include "SchematicNode.h"

const int InsertNodeButton = 20;


MainWindow::MainWindow()
{
    CreateActions();
    CreateToolBox();
    CreateMenus();

    m_scene = new SchematicScene(m_itemMenu, this);
    m_scene->setSceneRect(QRectF(0, 0, 5000, 5000));

    connect(m_scene, &SchematicScene::TextInserted,
            this, &MainWindow::TextInserted);

    connect(m_scene, &SchematicScene::NodeInserted,
            this, &MainWindow::NodeInserted);

    CreateToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_toolBox);
    m_view = new QGraphicsView(m_scene);
    layout->addWidget(m_view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle(tr("netlistviz"));
    setUnifiedTitleAndToolBarOnMac(true);
}


void MainWindow::ButtonGroupClicked(int id)
{
    const QList<QAbstractButton *> buttons = m_buttonGroup->buttons();
    for (QAbstractButton *button : buttons) {
        if (m_buttonGroup->button(id) != button)
            button->setChecked(false);
    }

    /* InsertNodeButton clicked */
    if (id == InsertNodeButton) {
        m_scene->SetMode(SchematicScene::InsertNodeMode);
    
    /* InsertDeviceButton clicked */
    } else {
        m_scene->SetMode(SchematicScene::InsertDeviceMode);
        m_scene->SetDeviceType(SchematicDevice::DeviceType(id));
    }

}


void MainWindow::DeleteItem()
{
    QList<QGraphicsItem *> selectedItems = m_scene->selectedItems();
    for (QGraphicsItem *item : qAsConst(selectedItems)) {
        if (item->type() == SchematicDevice::Type) {
            m_scene->removeItem(item);
            SchematicDevice *dev = qgraphicsitem_cast<SchematicDevice*>(item);
            dev->GetStartNode()->RemoveDevice(dev);
            dev->GetEndNode()->RemoveDevice(dev);
            delete item;
        }
    }
    selectedItems = m_scene->selectedItems();
    for (QGraphicsItem *item : qAsConst(selectedItems)) {
        if (item->type() == SchematicNode::Type) {
            qgraphicsitem_cast<SchematicNode*>(item)->RemoveDevices();
        }
        m_scene->removeItem(item);
        delete item;
    }
}


void MainWindow::PointerGroupClicked(int id)
{
    m_scene->SetMode(SchematicScene::Mode(m_pointerGroup->checkedId()));

    if (id == int(SchematicScene::BaseMode)) {

        m_pointerGroup->button(int(SchematicScene::BaseMode))->setChecked(true);
        m_pointerGroup->button(int(SchematicScene::InsertTextMode))->setChecked(false);

    } else if (id == int(SchematicScene::InsertTextMode)) {

        m_pointerGroup->button(int(SchematicScene::BaseMode))->setChecked(false);
        m_pointerGroup->button(int(SchematicScene::InsertTextMode))->setChecked(true);
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
    m_pointerGroup->button(int(SchematicScene::BaseMode))->setChecked(true);
    m_pointerGroup->button(int(SchematicScene::InsertTextMode))->setChecked(false);
    m_scene->SetMode(SchematicScene::BaseMode);
}


void MainWindow::NodeInserted(SchematicNode *)
{
    m_pointerGroup->button(int(SchematicScene::BaseMode))->setChecked(true);
    m_pointerGroup->button(int(SchematicScene::InsertTextMode))->setChecked(false);
    m_scene->SetMode(SchematicScene::BaseMode);

    m_buttonGroup->button(InsertNodeButton)->setChecked(false);
}


void MainWindow::CurrentFontChanged(const QFont &)
{
    HandleFontChange();
}


void MainWindow::FontSizeChanged(const QString &)
{
    HandleFontChange();
}


void MainWindow::SceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = m_view->matrix();
    m_view->resetMatrix();
    m_view->translate(oldMatrix.dx(), oldMatrix.dy());
    m_view->scale(newScale, newScale);
}


void MainWindow::TextColorChanged()
{
    m_textAction = qobject_cast<QAction *>(sender());
    m_fontColorToolButton->setIcon(CreateColorToolButtonIcon(
        ":/images/textpointer.png",
        qvariant_cast<QColor>(m_textAction->data())));
    TextButtonTriggered();
}


void MainWindow::NodeColorChanged()
{

}


void MainWindow::TextButtonTriggered()
{
    m_scene->SetTextColor(qvariant_cast<QColor>(m_textAction->data()));
}


void MainWindow::FillButtonTriggered()
{
    // scene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
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


void MainWindow::CreateToolBox()
{
    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(false);
    connect(m_buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &MainWindow::ButtonGroupClicked);

    QGridLayout *layout = new QGridLayout;

    /* Add device(s) */
    layout->addWidget(CreateCellWidget(tr("Resistor"), SchematicDevice::Resistor), 0, 0);
    layout->addWidget(CreateCellWidget(tr("Capacitor"), SchematicDevice::Capacitor), 0, 1);
    layout->addWidget(CreateCellWidget(tr("Inductor"), SchematicDevice::Inductor), 1, 0);
    layout->addWidget(CreateCellWidget(tr("ISource"), SchematicDevice::Isrc), 1, 1);
    layout->addWidget(CreateCellWidget(tr("VSource"), SchematicDevice::Vsrc), 2, 0);

    /* Add node */
    QToolButton *nodeButton = new QToolButton;
    nodeButton->setCheckable(true);
    m_buttonGroup->addButton(nodeButton, InsertNodeButton);
    SchematicNode node(m_itemMenu);
    QIcon icon(node.GetImage());
    nodeButton->setIcon(icon);
    nodeButton->setIconSize(QSize(50, 50));
    QGridLayout *nodeLayout = new QGridLayout;
    nodeLayout->addWidget(nodeButton, 0, 0, Qt::AlignHCenter);
    nodeLayout->addWidget(new QLabel(tr("Node")), 1, 0, Qt::AlignCenter);
    QWidget *nodeWidget = new QWidget;
    nodeWidget->setLayout(nodeLayout);
    layout->addWidget(nodeWidget, 2, 1);

    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

    //! [22]
    m_toolBox = new QToolBox;
    m_toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    m_toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    m_toolBox->addItem(itemWidget, tr("Device and Node"));
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
    m_deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(m_deleteAction, &QAction::triggered, this, &MainWindow::DeleteItem);

    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcuts(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("Quit Scenediagram example"));
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
    m_aboutAction->setShortcut(tr("F1"));
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::About);
}


void MainWindow::CreateMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_exitAction);

    m_itemMenu = menuBar()->addMenu(tr("&Item"));
    m_itemMenu->addAction(m_deleteAction);
    m_itemMenu->addSeparator();
    m_itemMenu->addAction(m_toFrontAction);
    m_itemMenu->addAction(m_sendBackAction);

    m_aboutMenu = menuBar()->addMenu(tr("&Help"));
    m_aboutMenu->addAction(m_aboutAction);
}


void MainWindow::CreateToolbars()
{
    m_editToolBar = addToolBar(tr("Edit"));
    m_editToolBar->addAction(m_deleteAction);
    m_editToolBar->addAction(m_toFrontAction);
    m_editToolBar->addAction(m_sendBackAction);

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


    // m_fillColorToolButton = new QToolButton;
    // m_fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    // m_fillColorToolButton->setMenu(CreateColorMenu(SLOT(itemColorChanged()), Qt::white));
    // m_fillAction = m_fillColorToolButton->menu()->defaultAction();
    // m_fillColorToolButton->setIcon(CreateColorToolButtonIcon(
    //     ":/images/floodfill.png", Qt::white));
    // connect(m_fillColorToolButton, &QAbstractButton::clicked,
    //         this, &MainWindow::FillButtonTriggered);

    m_textToolBar = addToolBar(tr("Font"));
    m_textToolBar->addWidget(m_fontCombo);
    m_textToolBar->addWidget(m_fontSizeCombo);
    m_textToolBar->addAction(m_boldAction);
    m_textToolBar->addAction(m_italicAction);
    m_textToolBar->addAction(m_underlineAction);

    m_colorToolBar = addToolBar(tr("Color"));
    m_colorToolBar->addWidget(m_fontColorToolButton);
    // m_colorToolBar->addWidget(m_fillColorToolButton);

    QToolButton *baseModePointerButton = new QToolButton;
    baseModePointerButton->setCheckable(true);
    baseModePointerButton->setChecked(true);
    baseModePointerButton->setIcon(QIcon(":/images/pointer.png"));

    QToolButton *insertTextPointerButton = new QToolButton;
    insertTextPointerButton->setCheckable(true);
    insertTextPointerButton->setIcon(QIcon(":/images/textpointer.png"));

    m_pointerGroup = new QButtonGroup(this);
    m_pointerGroup->setExclusive(true);
    m_pointerGroup->addButton(baseModePointerButton, int(SchematicScene::BaseMode));
    m_pointerGroup->addButton(insertTextPointerButton, int(SchematicScene::InsertTextMode));
    connect(m_pointerGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(PointerGroupClicked(int)));

    m_sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    m_sceneScaleCombo->addItems(scales);
    m_sceneScaleCombo->setCurrentIndex(2);
    connect(m_sceneScaleCombo, &QComboBox::currentTextChanged,
            this, &MainWindow::SceneScaleChanged);

    m_pointerToolbar = addToolBar(tr("Pointer type"));
    m_pointerToolbar->addWidget(baseModePointerButton);
    m_pointerToolbar->addWidget(insertTextPointerButton);
    m_pointerToolbar->addWidget(m_sceneScaleCombo);
}


QWidget *MainWindow::CreateCellWidget(const QString &text, SchematicDevice::DeviceType type)
{

    SchematicDevice device(type, nullptr, nullptr, nullptr);
    QIcon icon(device.GetImage());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    // m_buttonGroup->addButton(button, InsertDeviceButton);
    m_buttonGroup->addButton(button, type);

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


void MainWindow::ResetButtonAndCursor()
{

}
