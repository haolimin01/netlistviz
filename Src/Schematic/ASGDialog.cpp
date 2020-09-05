#include "ASGDialog.h"
#include <QDialogButtonBox>
#include <QButtonGroup>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QCheckBox>
#include <QDebug>
#include <QPushButton>

#include "Schematic/SchematicData.h"
#include "Schematic/SchematicDevice.h"
#include "Define/TypeDefine.h"

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

void ASGDialog::SetSchematicData(SchematicData *data)
{
    Q_ASSERT(data);
    m_data = data;

    CreatePropertyWidgets();
}

void ASGDialog::CreatePropertyWidgets()
{

    CreateFirstLevelWidget();

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(Accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    m_mainLayout->addStretch(1);
    m_mainLayout->addWidget(m_buttonBox);

}

void ASGDialog::CreateFirstLevelWidget()
{
    m_firstLevelSelectButtonGroup = new QButtonGroup();
    m_firstLevelSelectButtonGroup->setExclusive(false);

    QVBoxLayout *firstLevelSelectLayout = new QVBoxLayout;
    QWidget *firstLevelSelectWidget = new QWidget;
    firstLevelSelectWidget->setLayout(firstLevelSelectLayout);

    QHBoxLayout *firstLevelHeaderLayout = new QHBoxLayout;
    m_firstLevelSelectLabel = new QLabel(tr("Please select first level devices"));
    firstLevelHeaderLayout->addWidget(m_firstLevelSelectLabel);
    m_firstLevelSelectAllCheckBox = new QCheckBox(tr("Select All"));
    m_firstLevelSelectAllCheckBox->setChecked(false);
    connect(m_firstLevelSelectAllCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(FirstLevelSelectAllCheckBoxClicked(bool)));

    QLabel *dummyLabel = new QLabel;
    firstLevelHeaderLayout->addWidget(dummyLabel, 1);
    firstLevelHeaderLayout->addWidget(m_firstLevelSelectAllCheckBox);

    firstLevelSelectLayout->addLayout(firstLevelHeaderLayout);

    QGridLayout *firstLevelDeviceLayout = new QGridLayout;

    DeviceList deviceList = m_data->GetDeviceList();
    int colIndex = 0, rowIndex = 0;
    QCheckBox *ckBox = nullptr;
    foreach (SchematicDevice *dev, deviceList) {
        SchematicDevice::DeviceType t = dev->GetDeviceType();
        if (t != SchematicDevice::Vsrc && t != SchematicDevice::Isrc)  continue;
        if (NOT dev->MaybeAtFirstLevel())  continue;

        ckBox = new QCheckBox(dev->Name());
        ckBox->setChecked(false);
        ckBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        firstLevelDeviceLayout->addWidget(ckBox, rowIndex, colIndex);

        m_firstLevelSelectButtonGroup->addButton(ckBox, dev->Id());

        colIndex++;
        if (colIndex >= 5) {
            colIndex = 0;
            rowIndex++;
        }
    }

    connect(m_firstLevelSelectButtonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(FirstLevelSelectDeviceButtonGroupClicked(int)));

    firstLevelSelectLayout->addLayout(firstLevelDeviceLayout);
    QScrollArea *firstLevelSelectArea = new QScrollArea();
    firstLevelSelectArea->setWidget(firstLevelSelectWidget);
    m_mainLayout->addWidget(firstLevelSelectArea);
}

void ASGDialog::Accept()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    QAbstractButton *button = nullptr;
    QCheckBox *ckBox = nullptr;

    DeviceList checkedDeviceList;
    SchematicDevice *device = nullptr;

    foreach (button, m_firstLevelSelectButtonGroup->buttons()) {
        ckBox = static_cast<QCheckBox*>(button);
        if (ckBox->isChecked()) {
            qDebug() << ckBox->text() << endl;
            device = m_data->Device(ckBox->text());
            checkedDeviceList.push_back(device);
        }
    }

    m_data->SetFirstLevelDeviceList(checkedDeviceList);

    accept();
}

void ASGDialog::FirstLevelSelectAllCheckBoxClicked(bool checked)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    QAbstractButton *button = nullptr;
    QCheckBox *ckBox = nullptr;

    foreach (button, m_firstLevelSelectButtonGroup->buttons()) {
        ckBox = static_cast<QCheckBox*>(button);
        ckBox->setChecked(checked);
    }
}

void ASGDialog::FirstLevelSelectDeviceButtonGroupClicked(int)
{
    bool allSelected = true;

    QAbstractButton *button = nullptr;
    QCheckBox *ckBox = nullptr;

    foreach (button, m_firstLevelSelectButtonGroup->buttons()) {
        ckBox = static_cast<QCheckBox*>(button);
        if (NOT ckBox->isChecked()) {
            allSelected = false;
            break;
        }
    }

    if (allSelected)
        m_firstLevelSelectAllCheckBox->setChecked(true);
    else
        m_firstLevelSelectAllCheckBox->setChecked(false);
}