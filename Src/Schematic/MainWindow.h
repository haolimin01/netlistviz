#ifndef NETLISTVIZ_SCHEMATIC_MAINWINDOW_H
#define NETLISTVIZ_SCHEMATIC_MAINWINDOW_H

#include <QMainWindow>
#include "SchematicDevice.h"

class SchematicScene;
class SchematicNode;
class SchematicDevice;
class NetlistDialog;
class SchematicData;

QT_BEGIN_NAMESPACE
class QString;
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
class QRectF;
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

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

    /* Text inserted */
    void TextInserted(QGraphicsTextItem *item);

    /* Node inserted */
    void NodeInserted(SchematicNode *node);

    /* Device inserted */
    void DeviceInserted(SchematicDevice *device);

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
    void NodeButtonTriggered();

    /* Handle Font change */
    void HandleFontChange();

    /* Noed, Device, Text ... */
    void ItemSelected(QGraphicsItem *item);

    /* Project information */
    void About();

    /* Open netlist */
    void OpenNetlist();

    /* Open Schematic */
    void OpenSchematic();

    /* Plot Netlist to SchematicScene */
    void RenderNetlistFile();

    /* Save and SaveAs SchenaticFile */
    void SaveSchematicFile();
    void SaveAsSchematicFile();

    /* Update Window Title when scene changed */
    void UpdateWindowTitle(const QList<QRectF> &);

    /* Scroll action toggled */
    void ScrollActionToggled(bool checked);


private:
    /* Initialize variables */
    void InitVariables();

    /* Create Device, Node */
    void CreateToolBox();

    /* Create MainWindow actions */
    void CreateActions();

    /* Create MainWindow menus */
    void CreateMenus();

    /* Reset button in ToolBox and cursor */
    // void ResetButtonAndCursor();

    /* Create MainWindow toolbars */
    void CreateToolbars();

    /* Using by CreateToolBox, Create SchematicDevice */
    QWidget *CreateCellWidget(const QString &text,
                              SchematicDevice::DeviceType type);

    /* Color */
    QMenu *CreateColorMenu(const char *slot, QColor defaultColor);
    QIcon CreateColorToolButtonIcon(const QString &image, QColor color);
    QIcon CreateColorIcon(QColor color);

    void ShowNetlistFile(const QString &netlist);

    /* Parse Netlist to m_data */
    SchematicData* ParseNetlist();

    /* Critical Dialog */
    void ShowCriticalMsg(const QString &msg);



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

    QToolBar *m_fileToolbar;
    QToolBar *m_textToolBar;
    QToolBar *m_editToolBar;
    QToolBar *m_colorToolBar;
    QToolBar *m_pointerToolbar;

    QComboBox *m_sceneScaleCombo;
    QComboBox *m_fontSizeCombo;
    QFontComboBox *m_fontCombo;

    /* Node, Device */
    QToolBox *m_toolBox;

    /* Node, Device buttons */
    QButtonGroup *m_buttonGroup;

    /* Pointer buttons, BaseMode and InsertText Mode */
    QButtonGroup *m_pointerGroup;

    QToolButton *m_fontColorToolButton;
    QToolButton *m_nodeColorToolButton;

    QAction *m_boldAction;
    QAction *m_underlineAction;
    QAction *m_italicAction;
    QAction *m_textAction;
    QAction *m_nodeAction;

    QAction *m_scrollPointerAction;

    QAction *m_openNetlistAction;

    /* Save and SaveAs Schematic File */
    QAction *m_saveSchematicFileAction;
    QAction *m_saveAsSchematicFileAction;

    /* Open Schematic File */
    QAction *m_openSchematicFileAction;

    QString m_curNetlistPath;
    QString m_curNetlistFile;

    QString m_curSchematicPath;
    QString m_curSchematicFile;

    NetlistDialog *m_netlistDialog;

};
//! [0]

#endif // NETLISTVIZ_SCHEMATIC_MAINWINDOW_H
