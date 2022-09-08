#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "setting.h"

#include <dicombase.h>

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


void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    static bool init = false;
    if (!init)
    {
        init = true;
        loadSetting();
    }
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

    // Log
    ui->textEditLog->clear();
    ui->textEditLog->setStyleSheet("font: 9pt;");

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
    QList<int> connectionTimeout = { -1, 3, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
    QList<int> ACSETimeout = { 3, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
    QList<int> DIMSETimeout = { -1, 3, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
    QList<int> MaxPDU = { 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128 };
    QList<int> CompressionLevel = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    ui->comboBoxConnectionTimeout->clear();
    ui->comboBoxACSETimeout->clear();
    ui->comboBoxDIMSETimeout->clear();
    ui->comboBoxMaxPDU->clear();
    ui->comboBoxCompressionLevel->clear();

    // Stop When Fail;
    ui->checkBoxStopWhenError->setChecked(Setting::getStopWhenError());

    // Connection Timeout
    for (auto &value : connectionTimeout)
    {
        if (value <= 0)
        {
            ui->comboBoxConnectionTimeout->addItem("Unlimited", -1);
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
        index = 3;
    }
    ui->comboBoxACSETimeout->setCurrentIndex(index);

    // DIMSE Timeout
    for (auto &value : DIMSETimeout)
    {
        if (value <= 0)
        {
            ui->comboBoxDIMSETimeout->addItem("Unlimited", 0);
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

    // Other group
    updateOtherSetting(ui->radioDcmtk->isChecked());
}

void MainWindow::saveSetting()
{
    Setting::savePaths(m_paths);
    Setting::saveLocalAE(ui->editLocalAE->text().trimmed());
    Setting::saveTargetAE(ui->editTargetAE->text().trimmed());
    Setting::saveHost(ui->editHost->text().trimmed());
    Setting::savePort(ui->spinBoxPort->value());
    Setting::saveLibrary(ui->radioDcmtk->isChecked() ? "dcmtk" : "gdcm");
    Setting::saveStopWhenError(ui->checkBoxStopWhenError->isChecked());
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

void MainWindow::addLog(const QString &log)
{
    ui->textEditLog->moveCursor(QTextCursor::End);
    ui->textEditLog->insertPlainText(log + "\r\n");
    ui->textEditLog->moveCursor(QTextCursor::End);
}

void MainWindow::addColorLog(const bool result, const QString &log)
{
    ui->textEditLog->moveCursor(QTextCursor::End);
    auto textColor = ui->textEditLog->textColor();
    ui->textEditLog->setTextColor(result ? QColor(0, 165, 0) : Qt::red);
    ui->textEditLog->insertPlainText(result ? "[OK]" : "[ERROR]");
    ui->textEditLog->setTextColor(textColor);
    ui->textEditLog->insertPlainText(log + "\r\n");
    ui->textEditLog->moveCursor(QTextCursor::End);
}

void MainWindow::logDone()
{
    addLog("----Done----\r\n");
}

void MainWindow::updateOtherSetting(const bool isDcmtk)
{
    ui->comboBoxConnectionTimeout->setEnabled(true);
    ui->comboBoxACSETimeout->setEnabled(isDcmtk ? true : false);
    ui->comboBoxDIMSETimeout->setEnabled(isDcmtk ? true : false);
    ui->comboBoxMaxPDU->setEnabled(isDcmtk ? true : false);
    ui->comboBoxCompressionLevel->setEnabled(isDcmtk ? true : false);
}

bool MainWindow::validInput(CheckInput type)
{
    bool valid = true;

    switch (type) {
    case CheckInput::path:
        if (ui->comboboxPaths->currentText().isEmpty())
        {
            addColorLog(false, " No folder selected");
            valid = false;
        }
        break;
    case CheckInput::localAE:
        if (ui->editLocalAE->text().trimmed().isEmpty())
        {
            addColorLog(false, " Local AE is not valid");
            valid = false;
        }
        break;
    case CheckInput::targetAE:
        if (ui->editTargetAE->text().trimmed().isEmpty())
        {
            addColorLog(false, " Target AE is not valid");
            valid = false;
        }
        break;
    case CheckInput::host:
        if (ui->editHost->text().trimmed().isEmpty())
        {
            addColorLog(false, " Host is not valid");
            valid = false;
        }
        break;
    default:
        valid = false;
        break;
    }

    return valid;
}

void MainWindow::send()
{
    if (m_sendFiles.isEmpty())
    {
        ui->buttonSend->setEnabled(true);
        addColorLog(false, " No files to send");
        logDone();
        return;
    }

    auto store = new StoreThread(ui->radioDcmtk->isChecked() ? Library::dcmtk : Library::gdcm, this);
    store->object()->setFiles(m_sendFiles);
    store->object()->setLocalAE(ui->editLocalAE->text().trimmed());
    store->object()->setTargetAE(ui->editTargetAE->text().trimmed());
    store->object()->setHost(ui->editHost->text().trimmed());
    store->object()->setPort(ui->spinBoxPort->value());
    store->object()->setStopWhenError(ui->checkBoxStopWhenError->isChecked());
    store->object()->setConnectionTimeout(ui->comboBoxConnectionTimeout->currentData().toInt());
    if (ui->radioDcmtk->isChecked())
    {
        store->object()->setACSETimeout(ui->comboBoxACSETimeout->currentData().toInt());
        store->object()->setDIMSETimeout(ui->comboBoxDIMSETimeout->currentData().toInt());
        store->object()->setMaxPDU(ui->comboBoxMaxPDU->currentData().toInt());
        store->object()->setCompressionLevel(ui->comboBoxCompressionLevel->currentData().toInt());
    }

    auto fnDone = [&](const bool result, const QString &log)
    {
        if (!log.isEmpty())
        {
            addColorLog(result, " " + log);
        }
    };

    auto fnProcessed = [&](const QPair<int, int> &index, const bool result, const QString &log)
    {
        qDebug() << index.first << "/" << index.second << " " << log;
        QString totalStr = QString("%1").arg(index.second);
        QString str = QString("[%1/%2] %3").arg(index.first, totalStr.length(), 10, QChar('0')).arg(index.second).arg(log);

        addColorLog(result, str);
    };

    auto fnFinished = [&]()
    {
        ui->buttonSend->setEnabled(true);
        logDone();
    };

    connect(store, &StoreThread::done, this, fnDone);
    connect(store, &StoreThread::processed, this, fnProcessed);
    connect(store, &StoreThread::finished, this, fnFinished);
    connect(store, &StoreThread::finished, store, &QObject::deleteLater);
    store->start();
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

void MainWindow::onDcmtk()
{
    updateOtherSetting(ui->radioDcmtk->isChecked());
}

void MainWindow::onGdcm()
{
    updateOtherSetting(ui->radioDcmtk->isChecked());
}

void MainWindow::onEcho()
{
    if (!validInput(CheckInput::localAE) ||
        !validInput(CheckInput::targetAE) ||
        !validInput(CheckInput::host))
    {
        logDone();
        return;
    }

    auto echo = new EchoThread(ui->radioDcmtk->isChecked() ? Library::dcmtk : Library::gdcm, this);
    echo->object()->setLocalAE(ui->editLocalAE->text().trimmed());
    echo->object()->setTargetAE(ui->editTargetAE->text().trimmed());
    echo->object()->setHost(ui->editHost->text().trimmed());
    echo->object()->setPort(ui->spinBoxPort->value());
    echo->object()->setConnectionTimeout(ui->comboBoxConnectionTimeout->currentData().toInt());

    auto fnDone = [&](const bool result, const QString &log)
    {
        qDebug() << result << log;
        addColorLog(result, " " + log);
    };

    auto fnStarted = [&]()
    {
        ui->buttonEcho->setEnabled(false);
    };

    auto fnFinished = [&]()
    {
        ui->buttonEcho->setEnabled(true);
        logDone();
    };

    connect(echo, &EchoThread::done, this, fnDone);
    connect(echo, &EchoThread::started, this, fnStarted);
    connect(echo, &EchoThread::finished, this, fnFinished);
    connect(echo, &EchoThread::finished, echo, &QObject::deleteLater);
    echo->start();
}

void MainWindow::onSend()
{
    if (!validInput(CheckInput::path) || !validInput(CheckInput::localAE) ||
        !validInput(CheckInput::targetAE) || !validInput(CheckInput::host))
    {
        logDone();
        return;
    }

    auto scan = new ScanThread(ui->radioDcmtk->isChecked() ? Library::dcmtk : Library::gdcm, this);
    scan->object()->setDir(ui->comboboxPaths->currentText());
    scan->object()->setOutput(m_sendFiles);

    auto fnDone = [&](const int count)
    {
        qDebug() << "File(s): " << count;
        send();
    };

    auto fnStarted = [&]()
    {
        ui->buttonSend->setEnabled(false);
    };

    connect(scan, &ScanThread::done, this, fnDone);
    connect(scan, &ScanThread::started, this, fnStarted);
    connect(scan, &ScanThread::finished, scan, &QObject::deleteLater);
    scan->start();
}
