#ifndef NETLISTVIZ_SCHEMATIC_MAINWINDOW_H
#define NETLISTVIZ_SCHEMATIC_MAINWINDOW_H

#include <QMainWindow>
#include "Schematic/SchematicDevice.h"

class SchematicScene;
class SchematicDevice;
class NetlistDialog;
class ASG;
class ASGDialog;
class CircuitGraph;
class SchematicView;

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
    void DeviceBtnGroupClicked(int id);

    /* Delete wire, device, text ... */
    void DeleteItem();

    /* BaseMode and InsertText mode */
    void PointerBtnGroupClicked(int id);

    /* Change ZValue */
    void BringToFront();
    void SendToBack();

    /* Text inserted */
    void TextInserted(QGraphicsTextItem *item);

    /* Device inserted */
    void DeviceInserted(SchematicDevice *device);

    /* Font format */
    void CurrentFontChanged(const QFont &font);
    void FontSizeChanged(const QString &size);

    /* Enable zoom scene by wheel */
    void ZoomActionToggled(bool enable);

    /* set SchematicView scale to 1 */
    void ScaleToOriginTriggered();

    /* move to center */
    void MoveToCenterTriggered();

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

    /* Save and SaveAs SchenaticFile */
    void SaveSchematicFile();
    void SaveAsSchematicFile();

    /* Update Window Title when scene changed */
    void UpdateWindowTitle(const QList<QRectF> &);

    /* Scroll action toggled */
    void ScrollActionToggled(bool checked);

    /* Show or Hide terminal on items */
    void ShowItemTerminalToggled(bool show);

    /* Show as branch on wires and devices */
    void ShowBranchToggled(bool show);

    void ShowGridToggled(bool show);

    void NetlistChangedSlot();

    /* Parse Netlist to m_data */
    void ParseNetlist();

    /* For ASG */
    void ASGPropertyTriggered();
    void LogicalPlacement();
    void LogicalRouting();
    void GeometricalPlacement();
    void GeometricalRouting();


signals:
    void NetlistChanged();

protected:
    void closeEvent(QCloseEvent *event) override;

private:

    /* Initialize variables */
    void InitVariables();
    
    /* Create SchematicScene and connect its signals */
    void CreateSchematicScene();

    /* Create Device */
    void CreateDeviceToolBox();

    /* Create MainWindow actions */
    void CreateActions();

    /* Create MainWindow menus */
    void CreateMenus();

    /* Create Center Widget */
    void CreateCenterWidget();

    /* Reset button in ToolBox and cursor */
    // void ResetButtonAndCursor();

    /* Create MainWindow toolbars */
    void CreateToolBars();

    /* Using by CreateToolBox, Create SchematicDevice */
    QWidget *CreateCellWidget(const QString &text,
                              DeviceType type);

    /* Color */
    QMenu *CreateColorMenu(const char *slot, QColor defaultColor);
    QIcon CreateColorToolButtonIcon(const QString &image, QColor color);
    QIcon CreateColorIcon(QColor color);

    void ShowNetlistFile(const QString &netlist);


    /* Critical Dialog */
    void ShowCriticalMsg(const QString &msg);
    void ShowInfoMsg(const QString &msg);



    /* m menas member in class */

    /* The scene */
    SchematicScene     *m_scene;

    /* The view */
    SchematicView      *m_view;

    QAction            *m_exitAction;
    QAction            *m_deleteAction;

    QAction            *m_toFrontAction;
    QAction            *m_sendBackAction;
    QAction            *m_aboutAction;

    QMenu              *m_fileMenu;
    QMenu              *m_editMenu;
    QMenu              *m_viewMenu;
    QMenu              *m_aboutMenu;
    QMenu              *m_asgMenu;

    QToolBar           *m_fileToolBar;
    QToolBar           *m_textToolBar;
    QToolBar           *m_editToolBar;
    QToolBar           *m_colorToolBar;
    QToolBar           *m_pointerToolBar;
    QToolBar           *m_asgToolBar;

    QComboBox          *m_fontSizeCombo;
    QFontComboBox      *m_fontCombo;

    /* Device */
    QToolBox           *m_deviceToolBox;

    /* Device buttons */
    QButtonGroup       *m_deviceBtnGroup;

    /* Pointer buttons, BaseMode and InsertText Mode */
    QButtonGroup       *m_pointerBtnGroup;

    QToolButton        *m_fontColorToolButton;
    // QToolButton *m_wireColorToolButton;

    QAction            *m_boldAction;
    QAction            *m_underlineAction;
    QAction            *m_italicAction;
    QAction            *m_textAction;
    QAction            *m_showTerminalAction;
    QAction            *m_showBranchAction;
    QAction            *m_showGridAction;
    QAction            *m_zoomAction;
    QAction            *m_moveToCenterAction;
    QAction            *m_scaleToOriginAction;
    QAction            *m_scrollPointerAction;
    QAction            *m_openNetlistAction;
    QAction            *m_parseNetlistAction;
    /* Save and SaveAs Schematic File */
    QAction            *m_saveSchematicFileAction;
    QAction            *m_saveAsSchematicFileAction;
    /* Open Schematic File */
    QAction            *m_openSchematicFileAction;

    /* For ASG Actions */
    QAction            *m_asgPropertyAction;
    QAction            *m_logPlaceAction;
    QAction            *m_logRouteAction;
    QAction            *m_geoPlaceAction;
    QAction            *m_geoRouteAction;

    /* Device Panel */
    QDockWidget        *m_devicePanelDockWidget;

    QString             m_curNetlistPath;
    QString             m_curNetlistFile;
    QString             m_curSchematicPath;
    QString             m_curSchematicFile;

    NetlistDialog      *m_netlistDialog;
    CircuitGraph       *m_ckt;
    ASG                *m_asg;
    ASGDialog          *m_asgDialog;
    bool                m_asgPropertySelected;

    /* for cursor image */
    SchematicDevice    *m_deviceBeingAdded;
};

#endif // NETLISTVIZ_SCHEMATIC_MAINWINDOW_H
