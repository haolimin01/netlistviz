#include "ASGDialog.h"
#include <QDialogButtonBox>
#include <QButtonGroup>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QCheckBox>
#include <QDebug>

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
    m_firstLevelSelectionButtonGroup = new QButtonGroup();
    m_firstLevelSelectionButtonGroup->setExclusive(false);

    QVBoxLayout *firstLevelSelectionLayout = new QVBoxLayout;
    QWidget *firstLevelSelectionWidget = new QWidget;
    firstLevelSelectionWidget->setLayout(firstLevelSelectionLayout);
    m_firstLevelSelectionLabel = new QLabel(tr("Please select first level devices"));
    firstLevelSelectionLayout->addWidget(m_firstLevelSelectionLabel);

    QGridLayout *firstLevelDeviceLayout = new QGridLayout;
    // firstLevelDeviceLayout->setSpacing(10);

    DeviceList deviceList = m_data->GetDeviceList();
    int colIndex = 0, rowIndex = 0;
    QCheckBox *ckBox = nullptr;
    foreach (SchematicDevice *dev, deviceList) {
        SchematicDevice::DeviceType t = dev->GetDeviceType();
        if (t != SchematicDevice::Vsrc && t != SchematicDevice::Isrc)  continue;

        ckBox = new QCheckBox(dev->Name());
        ckBox->setChecked(false);
        ckBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        firstLevelDeviceLayout->addWidget(ckBox, rowIndex, colIndex);

        m_firstLevelSelectionButtonGroup->addButton(ckBox, dev->Id());

        colIndex++;
        if (colIndex >= 5) {
            colIndex = 0;
            rowIndex++;
        }
    }

    firstLevelSelectionLayout->addLayout(firstLevelDeviceLayout);
    QScrollArea *firstLevelSelectionArea = new QScrollArea();
    firstLevelSelectionArea->setWidget(firstLevelSelectionWidget);
    m_mainLayout->addWidget(firstLevelSelectionArea);
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

    foreach (button, m_firstLevelSelectionButtonGroup->buttons()) {
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