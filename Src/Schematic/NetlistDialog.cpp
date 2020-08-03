#include "NetlistDialog.h"
#include <QtWidgets>
#include <QVBoxLayout>
#include <QDebug>
#include <QFile>
#include "Define/Define.h"


NetlistDialog::NetlistDialog(QWidget *parent)
    : QDialog(parent)
{
    m_netlist = "";
    m_content = "";

    m_textEdit = new QTextEdit;
    m_textEdit->setReadOnly(true);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &NetlistDialog::OkButtonClicked);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &NetlistDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(m_textEdit);
    mainLayout->addWidget(m_buttonBox);
    setLayout(mainLayout);

    resize(800, 600);
}

void NetlistDialog::SetNetlistFile(const QString &netlist)
{
    m_netlist = netlist;
    setWindowTitle(QFileInfo(m_netlist).baseName());

    int error = ReadNetlist();
    if (error) {
        qDebug() << LINE_INFO << "Open " << m_netlist << " failed.";
        m_content = "";
    }

    m_textEdit->setText(QString(m_content));
}

int NetlistDialog::ReadNetlist()
{
    QFile file(m_netlist);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!file.isOpen()) {
        return ERROR;
    }

    m_content = file.readAll();
    file.close();
    return OKAY;
}

void NetlistDialog::OkButtonClicked()
{
    accept();
    emit Accepted();
}


