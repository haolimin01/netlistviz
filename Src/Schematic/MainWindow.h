#ifndef NETLISTVIZ_SCHEMATIC_MAINWINDOW_H
#define NETLISTVIZ_SCHEMATIC_MAINWINDOW_H

#include <QMainWindow>
#include "SchematicDevice.h"

class SchematicScene;
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
    /* Device*/
    void ButtonGroupClicked(int id);

    /* Delete wire, device, text ... */
    void DeleteItem();

    /* BaseMode and InsertText mode */
    void PointerGroupClicked(int);

    /* Change ZValue */
    void BringToFront();
    void SendToBack();

    /* Text inserted */
    void TextInserted(QGraphicsTextItem *item);

    /* Wire inserted */
    // void WireInserted(SchematicWire *wire);

    /* Device inserted */
    void DeviceInserted(SchematicDevice *device);

    /* Font format */
    void CurrentFontChanged(const QFont &font);
    void FontSizeChanged(const QString &size);

    /* SchematicScene scale changed */
    void SceneScaleChanged(const QString &scale);

    /* Text color changed */
    void TextColorChanged();

    /* Wire color changed */
    // void WireColorChanged();

    /* Text Button triggered */
    void TextButtonTriggered();

    /* Fill Button triggered */
    // void WireButtonTriggered();

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

    /* Show or Hide node on items */
    void ShowItemNodeToggled(bool show);


private:
    /* Initialize variables */
    void InitVariables();
    
    /* Create SchematicScene and connect its signals */
    void CreateSchematicScene();

    /* Create Device */
    void CreateToolBox();

    /* Create MainWindow actions */
    void CreateActions();

    /* Create MainWindow menus */
    void CreateMenus();

    /* Create Center Widget */
    void CreateCenterWidget();

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
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_aboutMenu;

    QToolBar *m_fileToolbar;
    QToolBar *m_textToolBar;
    QToolBar *m_editToolBar;
    QToolBar *m_colorToolBar;
    QToolBar *m_pointerToolbar;

    QComboBox *m_sceneScaleCombo;
    QComboBox *m_fontSizeCombo;
    QFontComboBox *m_fontCombo;

    /* Device */
    QToolBox *m_toolBox;

    /* Device buttons */
    QButtonGroup *m_buttonGroup;

    /* Pointer buttons, BaseMode and InsertText Mode */
    QButtonGroup *m_pointerGroup;

    QToolButton *m_fontColorToolButton;
    // QToolButton *m_wireColorToolButton;

    QAction *m_boldAction;
    QAction *m_underlineAction;
    QAction *m_italicAction;
    QAction *m_textAction;
    // QAction *m_wireAction;
    QAction *m_showNodeAction;

    QAction *m_scrollPointerAction;

    QAction *m_openNetlistAction;

    /* Save and SaveAs Schematic File */
    QAction *m_saveSchematicFileAction;
    QAction *m_saveAsSchematicFileAction;

    /* Open Schematic File */
    QAction *m_openSchematicFileAction;

    /* Device Panel */
    QDockWidget *m_devicePanelDockWidget;

    QString m_curNetlistPath;
    QString m_curNetlistFile;

    QString m_curSchematicPath;
    QString m_curSchematicFile;

    NetlistDialog *m_netlistDialog;

};
//! [0]

#endif // NETLISTVIZ_SCHEMATIC_MAINWINDOW_H
