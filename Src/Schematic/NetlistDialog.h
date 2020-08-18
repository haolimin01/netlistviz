#ifndef NETLISTVIZ_SCHEMATIC_NETLIST_DIALOG_H
#define NETLISTVIZ_SCHEMATIC_NETLIST_DIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QByteArray>


class NetlistDialog : public QDialog
{
    // Q_OBJECT

public:
    NetlistDialog(QWidget *parent = 0);

    void SetNetlistFile(const QString &netlist);

private:

    int ReadNetlist();

    QString    m_netlist;
    QByteArray m_content;

    QTextEdit *m_textEdit;
    QDialogButtonBox *m_buttonBox;
};


#endif // NETLISTVIZ_SCHEMATIC_NETLIST_DIALOG_H