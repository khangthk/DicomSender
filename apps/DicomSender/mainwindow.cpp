#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "setting.h"

#include <QString>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMimeData>

#define MAX_PATH 10

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    loadSetting();

    connect(ui->buttonBrowse, &QPushButton::clicked, this, &MainWindow::onBrowse);
    connect(ui->radioDcmtk, &QAbstractButton::clicked, this, &MainWindow::onDcmtk);
    connect(ui->radioGdcm, &QAbstractButton::clicked, this, &MainWindow::onGdcm);
    connect(ui->buttonEcho, &QPushButton::clicked, this, &MainWindow::onEcho);
    connect(ui->buttonSend, &QPushButton::clicked, this, &MainWindow::onSend);
}

MainWindow::~MainWindow()
{
    saveSetting();
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    auto files = mimeData->urls();
    if (files.size() != 1)
    {
        return;
    }

    auto file = files.at(0).toLocalFile();
    if (!QFileInfo(file).isDir())
    {
        return;
    }

    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (!mimeData->hasUrls() || mimeData->urls().size() != 1)
    {
        return;
    }

    event->acceptProposedAction();

    addPath(mimeData->urls().at(0).toLocalFile());
}

void MainWindow::onBrowse()
{
    QString lastPath = (m_paths.empty() ? QDir::currentPath() : ui->comboboxPaths->currentText());
    QString dir = QFileDialog::getExistingDirectory(this, "Select Dicom Folder", lastPath,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        addPath(dir);
    }
}

void MainWindow::onDcmtk(bool checked)
{
    qDebug() << "dcmtk:" << checked;
    if (checked)
    {
        ui->groupBoxOther->setEnabled(true);
    }
}

void MainWindow::onGdcm(bool checked)
{
    qDebug() << "gdcm:" << checked;
    if (checked)
    {
        ui->groupBoxOther->setEnabled(false);
    }
}

void MainWindow::onEcho()
{

}

void MainWindow::onSend()
{

}

void MainWindow::loadSetting()
{
    // Path list
    m_paths = Setting::getPaths();
    ui->comboboxPaths->clear();
    ui->comboboxPaths->addItems(m_paths);
    if (!m_paths.isEmpty())
    {
        ui->comboboxPaths->setCurrentIndex(0);
    }

    // Setting group
    ui->editLocalAE->setText(Setting::getLocalAE());
    ui->editTargetAE->setText(Setting::getTargetAE());
    ui->editHost->setText(Setting::getHost());
    ui->spinBoxPort->setValue(Setting::getPort());

    // Library group
    ui->radioDcmtk->setChecked(Setting::getLibrary() == "dcmtk");
    ui->radioGdcm->setChecked(Setting::getLibrary() == "gdcm");
    if (!ui->radioDcmtk->isChecked() && !ui->radioGdcm->isChecked())
    {
        ui->radioDcmtk->setChecked(true);
    }

    // Other group
    QList<int> connectionTimeout = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
    QList<int> ACSETimeout = { 10, 20, 30, 40, 50, 60, 70, 80, 90 };
    QList<int> DIMSETimeout = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
    QList<int> MaxPDU = { 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128 };
    QList<int> CompressionLevel = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    ui->comboBoxConnectionTimeout->clear();
    ui->comboBoxACSETimeout->clear();
    ui->comboBoxDIMSETimeout->clear();
    ui->comboBoxMaxPDU->clear();
    ui->comboBoxCompressionLevel->clear();

    // Connection Timeout
    for (auto &value : connectionTimeout)
    {
        if (value == 0)
        {
            ui->comboBoxConnectionTimeout->addItem("Unlimited", value);
        }
        else
        {
            ui->comboBoxConnectionTimeout->addItem(QString("%1 seconds").arg(value), value);
        }
    }
    int index = connectionTimeout.indexOf(Setting::getConnectionTimeout());
    if (index == -1)
    {
        index = 0;
    }
    ui->comboBoxConnectionTimeout->setCurrentIndex(index);

    // ACSE Timeout
    for (auto &value : ACSETimeout)
    {
        ui->comboBoxACSETimeout->addItem(QString("%1 seconds").arg(value), value);
    }
    index = ACSETimeout.indexOf(Setting::getACSETimeout());
    if (index == -1)
    {
        index = 2;
    }
    ui->comboBoxACSETimeout->setCurrentIndex(index);

    // DIMSE Timeout
    for (auto &value : DIMSETimeout)
    {
        if (value == 0)
        {
            ui->comboBoxDIMSETimeout->addItem("Unlimited", value);
        }
        else
        {
            ui->comboBoxDIMSETimeout->addItem(QString("%1 seconds").arg(value), value);
        }
    }
    index = DIMSETimeout.indexOf(Setting::getDIMSETimeout());
    if (index == -1)
    {
        index = 0;
    }
    ui->comboBoxDIMSETimeout->setCurrentIndex(index);

    // Max PDU
    for (auto &value : MaxPDU)
    {
        ui->comboBoxMaxPDU->addItem(QString("%1K (%2)").arg(value).arg(value * 1024), value);
    }
    index = MaxPDU.indexOf(Setting::getMaxPDU());
    if (index == -1)
    {
        index = 3;
    }
    ui->comboBoxMaxPDU->setCurrentIndex(index);

    // Compression Level
    for (auto &value : CompressionLevel)
    {
        ui->comboBoxCompressionLevel->addItem(QString("%1").arg(value), value);
    }
    index = CompressionLevel.indexOf(Setting::getCompressionLevel());
    if (index == -1)
    {
        index = 6;
    }
    ui->comboBoxCompressionLevel->setCurrentIndex(index);

    // Enable/Disable Other group
    ui->groupBoxOther->setEnabled(ui->radioDcmtk->isChecked());
}

void MainWindow::saveSetting()
{
    Setting::savePaths(m_paths);
    Setting::saveLocalAE(ui->editLocalAE->text());
    Setting::saveTargetAE(ui->editTargetAE->text());
    Setting::saveHost(ui->editHost->text());
    Setting::savePort(ui->spinBoxPort->value());
    Setting::saveLibrary(ui->radioDcmtk->isChecked() ? "dcmtk" : "gdcm");
    Setting::saveConnectionTimeout(ui->comboBoxConnectionTimeout->currentData().toInt());
    Setting::saveACSETimeout(ui->comboBoxACSETimeout->currentData().toInt());
    Setting::saveDIMSETimeout(ui->comboBoxDIMSETimeout->currentData().toInt());
    Setting::saveMaxPDU(ui->comboBoxMaxPDU->currentData().toInt());
    Setting::saveCompressionLevel(ui->comboBoxCompressionLevel->currentData().toInt());
}

void MainWindow::addPath(const QString &path)
{
    m_paths.push_front(QDir::toNativeSeparators(path));
    m_paths.removeDuplicates();

    while (m_paths.size() > MAX_PATH)
    {
        m_paths.removeLast();
    }

    ui->comboboxPaths->clear();
    ui->comboboxPaths->addItems(m_paths);
    ui->comboboxPaths->setCurrentIndex(0);
}
