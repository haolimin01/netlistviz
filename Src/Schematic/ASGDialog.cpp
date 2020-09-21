#include "ASGDialog.h"
#include <QDialogButtonBox>
#include <QButtonGroup>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QCheckBox>
#include <QDebug>
#include <QPushButton>

#include "Circuit/CircuitGraph.h"
#include "Circuit/Device.h"
#include "Define/TypeDefine.h"
#include "ASG/ASG.h"

ASGDialog::ASGDialog(QWidget *parent)
    : QDialog(parent)
{
    m_mainLayout = new QVBoxLayout();
    setWindowTitle(tr("ASG Preferences"));
    resize(800, 600);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setLayout(m_mainLayout);
}

ASGDialog::~ASGDialog()
{

}

void ASGDialog::SetCircuitGraph(CircuitGraph *ckt)
{
    Q_ASSERT(ckt);
    m_ckt = ckt;

    CreatePropertyWidgets();
}

void ASGDialog::SetASG(ASG *asg)
{
    Q_ASSERT(asg);
    m_asg = asg;
} 

void ASGDialog::CreatePropertyWidgets()
{

    CreateFLWidget();
    CreateICWidget();

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(Accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    m_mainLayout->addStretch(1);
    m_mainLayout->addWidget(m_buttonBox);
}

void ASGDialog::CreateFLWidget()
{
    m_flSelectButtonGroup = new QButtonGroup();
    m_flSelectButtonGroup->setExclusive(false);

    QVBoxLayout *flSelectLayout = new QVBoxLayout;
    QWidget *flSelectWidget = new QWidget;
    flSelectWidget->setLayout(flSelectLayout);

    QHBoxLayout *flHeaderLayout = new QHBoxLayout;
    QLabel *flSelectLabel = new QLabel(tr("Please select first level devices"));
    flHeaderLayout->addWidget(flSelectLabel);
    m_flSelectAllCheckBox = new QCheckBox(tr("Select All"));
    m_flSelectAllCheckBox->setChecked(false);
    connect(m_flSelectAllCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(FLSelectAllCheckBoxClicked(bool)));

    QLabel *dummyLabel = new QLabel;
    flHeaderLayout->addWidget(dummyLabel, 1);
    flHeaderLayout->addWidget(m_flSelectAllCheckBox);

    flSelectLayout->addLayout(flHeaderLayout);

    QGridLayout *flDeviceLayout = new QGridLayout;

    int colIndex = 0, rowIndex = 0;
    QCheckBox *ckBox = nullptr;

    foreach (Device *dev, m_ckt->GetDeviceList()) {
        DeviceType dtype = dev->GetDeviceType();
        if (NOT dev->MaybeAtFirstLevel())  continue;

        ckBox = new QCheckBox(dev->Name());
        ckBox->setChecked(false);
        ckBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        flDeviceLayout->addWidget(ckBox, rowIndex, colIndex);

        m_flSelectButtonGroup->addButton(ckBox, dev->Id());

        colIndex++;
        if (colIndex >= 5) {
            colIndex = 0;
            rowIndex++;
        }
    }

    connect(m_flSelectButtonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(FLSelectDeviceButtonGroupClicked(int)));

    flSelectLayout->addLayout(flDeviceLayout);
    QScrollArea *flSelectArea = new QScrollArea();
    flSelectArea->setWidget(flSelectWidget);
    m_mainLayout->addWidget(flSelectArea);
}

void ASGDialog::CreateICWidget()
{
    m_icButtonGroup = new QButtonGroup();
    m_icButtonGroup->setExclusive(true);

    QVBoxLayout *icLayout = new QVBoxLayout;
    QFrame *icFrame = new QFrame();
    icFrame->setStyleSheet(tr("border:1px"));
    QLabel *icLabel = new QLabel(tr("Please select cap(s) to be ignored"));
    icLayout->addWidget(icLabel);

    /* add checkboxes */
    QCheckBox *incCheckBox = new QCheckBox(tr("Ignore No Cap"));
    incCheckBox->setChecked(false);
    incCheckBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_icButtonGroup->addButton(incCheckBox, IgnoreNoCap);

    QCheckBox *igcCheckBox = new QCheckBox(tr("Ignore Ground Cap(s)"));
    igcCheckBox->setChecked(true);
    igcCheckBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_icButtonGroup->addButton(igcCheckBox, IgnoreGCap);

    QCheckBox *igccCheckBox = new QCheckBox(tr("Ignore Ground and Coupled Cap(s)"));
    igccCheckBox->setChecked(false);
    igccCheckBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_icButtonGroup->addButton(igccCheckBox, IgnoreGCCap);

    icLayout->addWidget(incCheckBox);
    icLayout->addWidget(igcCheckBox);
    icLayout->addWidget(igccCheckBox);
    icFrame->setLayout(icLayout);

    m_mainLayout->addWidget(icFrame);
}

void ASGDialog::Accept()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    QAbstractButton *button = nullptr;
    QCheckBox *ckBox = nullptr;

    /* For FirstLevelDeviceSelecion */
    ProcessFLSelectDeviceButtonGroup();

    /* For IgnoreCap */
    ProcessICButtonGroup();

    accept();
}

void ASGDialog::FLSelectAllCheckBoxClicked(bool checked)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    QAbstractButton *button = nullptr;
    QCheckBox *ckBox = nullptr;

    foreach (button, m_flSelectButtonGroup->buttons()) {
        ckBox = static_cast<QCheckBox*>(button);
        ckBox->setChecked(checked);
    }
}

void ASGDialog::FLSelectDeviceButtonGroupClicked(int)
{
    bool allSelected = true;

    QAbstractButton *button = nullptr;
    QCheckBox *ckBox = nullptr;

    foreach (button, m_flSelectButtonGroup->buttons()) {
        ckBox = static_cast<QCheckBox*>(button);
        if (NOT ckBox->isChecked()) {
            allSelected = false;
            break;
        }
    }

    if (allSelected)
        m_flSelectAllCheckBox->setChecked(true);
    else
        m_flSelectAllCheckBox->setChecked(false);
}

void ASGDialog::ProcessFLSelectDeviceButtonGroup()
{
    QAbstractButton *button = nullptr;
    QCheckBox *ckBox = nullptr;

    Device *device = nullptr;
    DeviceList  checkedDeviceList;

    foreach (button, m_flSelectButtonGroup->buttons()) {
        ckBox = static_cast<QCheckBox*>(button);
        if (ckBox->isChecked()) {
            device = m_ckt->GetDevice(ckBox->text());
            checkedDeviceList.push_back(device);
        }
    }

    m_ckt->SetFirstLevelDeviceList(checkedDeviceList);
}

void ASGDialog::ProcessICButtonGroup()
{
    int id = m_icButtonGroup->checkedId();
    IgnoreCap ignore = (IgnoreCap)(id);
    m_asg->SetIgnoreCapType(ignore);
}
