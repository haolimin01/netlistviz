#ifndef NETLISTVIZ_SCHEMATIC_ASG_DIALOG_H
#define NETLISTVIZ_SCHEMATIC_ASG_DIALOG_H

/*
 * @filename : ASGDialog.h
 * @author   : Hao Limin
 * @date     : 2020.08.13
 * @desp     : ASG property dialog.
 */

#include <QDialog>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QButtonGroup;
class QLabel;
QT_END_NAMESPACE;

class SchematicData;


class ASGDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ASGDialog(QWidget *parent = 0);
    ~ASGDialog();

    void SetSchematicData(SchematicData *data);

private slots:
    void Accept();

private:
    void CreatePropertyWidgets();
    void CreateFirstLevelWidget();

    QVBoxLayout   *m_mainLayout;
    QLabel        *m_firstLevelSelectionLabel;
    QButtonGroup  *m_firstLevelSelectionButtonGroup;
    SchematicData *m_data;

    QDialogButtonBox *m_buttonBox;
};

#endif // NETLISTVIZ_SCHEMATIC_ASG_DIALOG_H