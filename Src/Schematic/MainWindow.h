#ifndef NETLISTVIZ_SCHEMATIC_MAINWINDOW_H
#define NETLISTVIZ_SCHEMATIC_MAINWINDOW_H

#include <QMainWindow>
#include "SchematicDevice.h"

class SchematicScene;
class SchematicNode;
class SchematicDevice;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QGraphicsItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    /* Device, Node, Text */
    void ButtonGroupClicked(int id);

    /* Delete node, device, text ... */
    void DeleteItem();

    /* BaseMode and InsertText mode */
    void PointerGroupClicked(int);

    /* Change ZValue */
    void BringToFront();
    void SendToBack();

    /* Text insterted */
    void TextInserted(QGraphicsTextItem *item);

    /* Node insterted */
    void NodeInserted(SchematicNode *node);

    /* Font format */
    void CurrentFontChanged(const QFont &font);
    void FontSizeChanged(const QString &size);

    /* SchematicScene scale changed */
    void SceneScaleChanged(const QString &scale);

    /* Text color changed */
    void TextColorChanged();

    /* Node color changed */
    void NodeColorChanged();

    /* Text Button triggered */
    void TextButtonTriggered();

    /* Fill Button triggered */
    void FillButtonTriggered();

    /* Handle Font change */
    void HandleFontChange();

    /* Noed, Device, Text ... */
    void ItemSelected(QGraphicsItem *item);

    /* Project information */
    void About();

private:
    /* Create Device, Node */
    void CreateToolBox();

    /* Create MainWindow actions */
    void CreateActions();

    /* Create MainWindow menus */
    void CreateMenus();

    /* Reset button in ToolBox and cursor */
    void ResetButtonAndCursor();

    /* Create MainWindow toolbars */
    void CreateToolbars();

    /* Using by CreateToolBox, Create SchematicDevice */
    QWidget *CreateCellWidget(const QString &text,
                              SchematicDevice::DeviceType type);

    /* Color */
    QMenu *CreateColorMenu(const char *slot, QColor defaultColor);
    QIcon CreateColorToolButtonIcon(const QString &image, QColor color);
    QIcon CreateColorIcon(QColor color);


    /* m menas member in class */

    /* The scene */
    SchematicScene *m_scene;

    /* The view */
    QGraphicsView *m_view;

    QAction *m_exitAction;
    QAction *m_deleteAction;

    QAction *m_toFrontAction;
    QAction *m_sendBackAction;
    QAction *m_aboutAction;

    QMenu *m_fileMenu;
    QMenu *m_itemMenu;
    QMenu *m_aboutMenu;

    QToolBar *m_textToolBar;
    QToolBar *m_editToolBar;
    QToolBar *m_colorToolBar;
    QToolBar *m_pointerToolbar;

    QComboBox *m_sceneScaleCombo;
    QComboBox *m_nodeColorCombo;
    QComboBox *m_textColorCombo;
    QComboBox *m_fontSizeCombo;
    QFontComboBox *m_fontCombo;

    /* Node, Device */
    QToolBox *m_toolBox;

    /* Node, Device buttons */
    QButtonGroup *m_buttonGroup;

    /* Pointer buttons, BaseMode and InsertText Mode */
    QButtonGroup *m_pointerGroup;

    QToolButton *m_fontColorToolButton;
    QToolButton *m_fillColorToolButton;

    QAction *m_boldAction;
    QAction *m_underlineAction;
    QAction *m_italicAction;
    QAction *m_textAction;
    QAction *m_fillAction;
};
//! [0]

#endif // NETLISTVIZ_SCHEMATIC_MAINWINDOW_H