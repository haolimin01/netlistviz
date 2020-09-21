#ifndef NETLISTVIZ_SCHEMATIC_ASG_DIALOG_H
#define NETLISTVIZ_SCHEMATIC_ASG_DIALOG_H

/*
 * @filename : ASGDialog.h
 * @author   : Hao Limin
 * @date     : 2020.08.13
 * @desp     : ASG property dialog.
 * @modified : Hao Limin, 2020.09.12
 * @modified : Hao Limin, 2020.09.21
 */

/*
 * @terms:
 * FirstLevel             : FL
 * IgnoreCap              : IC
 * IgnoreGroundCap        : IGC
 * IgnoreCoupledCap       : ICC 
 * IgnoreGroundCoupledCap : IGCC
 */

#include <QDialog>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QButtonGroup;
class QLabel;
class QCheckBox;
QT_END_NAMESPACE;

class CircuitGraph;
class ASG;


class ASGDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ASGDialog(QWidget *parent = 0);
    ~ASGDialog();

    void SetCircuitGraph(CircuitGraph *ckt);
    void SetASG(ASG *asg);

private slots:
    void Accept();
    void FLSelectAllCheckBoxClicked(bool checked);
    void FLSelectDeviceButtonGroupClicked(int id);

private:
    void CreatePropertyWidgets();
    void CreateFLWidget();
    void CreateICWidget();

    /* called in Accept */
    void ProcessFLSelectDeviceButtonGroup();
    void ProcessICButtonGroup();

    /* For FirstLevelDeviceSelection */
    QVBoxLayout      *m_mainLayout;
    QButtonGroup     *m_flSelectButtonGroup;
    QCheckBox        *m_flSelectAllCheckBox;

    /* For GroundCap and CoupledCap */
    QButtonGroup     *m_icButtonGroup;

    QDialogButtonBox *m_buttonBox;

    /* Cicuit Graph */
    CircuitGraph     *m_ckt;
    ASG              *m_asg;
};

#endif // NETLISTVIZ_SCHEMATIC_ASG_DIALOG_H
