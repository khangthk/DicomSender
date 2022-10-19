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
    : QMainWindow(parent), ui(new Ui::MainWindow), m_files(nullptr)
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
    qDebug() << "~MainWindow";
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
    int index = -1;

    // Path list
    m_paths = Setting::getPaths();
    ui->comboBoxPaths->clear();
    ui->comboBoxPaths->addItems(m_paths);
    if (!m_paths.isEmpty())
    {
        index = m_paths.indexOf(Setting::getCurrentPath());
        if (index == -1)
        {
            index = 0;  // First path
        }
        ui->comboBoxPaths->setCurrentIndex(index);
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
    ui->radioDcmtk->setChecked(Setting::getLibrary() == Library::dcmtk);
    ui->radioGdcm->setChecked(Setting::getLibrary() == Library::gdcm);
    if (!ui->radioDcmtk->isChecked() && !ui->radioGdcm->isChecked())
    {
        ui->radioDcmtk->setChecked(true);
    }

    // Other group
    QList<int> connectionTimeout = { -1, 3, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };
    QList<int> socketTimeout = { -1, 0, 3, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };
    QList<int> ACSETimeout = { 3, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };
    QList<int> DIMSETimeout = { 0, 3, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };
    QList<int> MaxPDU = { 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128 };

    ui->comboBoxConnectionTimeout->clear();
    ui->comboBoxSocketTimeout->clear();
    ui->comboBoxACSETimeout->clear();
    ui->comboBoxDIMSETimeout->clear();
    ui->comboBoxMaxSendPDU->clear();
    ui->comboBoxMaxReceivePDU->clear();

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
    index = connectionTimeout.indexOf(Setting::getConnectionTimeout());
    if (index == -1)
    {
        index = 0;  // Unlimited
    }
    ui->comboBoxConnectionTimeout->setCurrentIndex(index);

    // Socket Timeout
    for (auto &value : socketTimeout)
    {
        if (value < 0)
        {
            ui->comboBoxSocketTimeout->addItem("No Setting", -1);
        }
        else if (value == 0)
        {
            ui->comboBoxSocketTimeout->addItem("Unlimited", 0);
        }
        else
        {
            ui->comboBoxSocketTimeout->addItem(QString("%1 seconds").arg(value), value);
        }
    }
    index = socketTimeout.indexOf(Setting::getSocketTimeout());
    if (index == -1)
    {
        index = 8;  // 60 seconds
    }
    ui->comboBoxSocketTimeout->setCurrentIndex(index);

    // ACSE Timeout
    for (auto &value : ACSETimeout)
    {
        ui->comboBoxACSETimeout->addItem(QString("%1 seconds").arg(value), value);
    }
    index = ACSETimeout.indexOf(Setting::getACSETimeout());
    if (index == -1)
    {
        index = 3;  // 30 seconds
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
        index = 0;  // Unlimited
    }
    ui->comboBoxDIMSETimeout->setCurrentIndex(index);

    // Max Send PDU
    for (auto &value : MaxPDU)
    {
        ui->comboBoxMaxSendPDU->addItem(QString("%1K (%2)").arg(value).arg(value * 1024), value);
    }
    index = MaxPDU.indexOf(Setting::getMaxSendPDU());
    if (index == -1)
    {
        index = 3;  // 16K (16384)
    }
    ui->comboBoxMaxSendPDU->setCurrentIndex(index);

    // Max Receive PDU
    for (auto &value : MaxPDU)
    {
        ui->comboBoxMaxReceivePDU->addItem(QString("%1K (%2)").arg(value).arg(value * 1024), value);
    }
    index = MaxPDU.indexOf(Setting::getMaxReceivePDU());
    if (index == -1)
    {
        index = 3;  // 16K (16384)
    }
    ui->comboBoxMaxReceivePDU->setCurrentIndex(index);

    // Other group
    updateOtherSetting(ui->radioDcmtk->isChecked());
}

void MainWindow::saveSetting()
{
    Setting::savePaths(m_paths);
    Setting::saveCurrentPath(ui->comboBoxPaths->currentText());
    Setting::saveLocalAE(ui->editLocalAE->text().trimmed());
    Setting::saveTargetAE(ui->editTargetAE->text().trimmed());
    Setting::saveHost(ui->editHost->text().trimmed());
    Setting::savePort(ui->spinBoxPort->value());
    Setting::saveLibrary(ui->radioDcmtk->isChecked() ? Library::dcmtk : Library::gdcm);
    Setting::saveStopWhenError(ui->checkBoxStopWhenError->isChecked());
    Setting::saveConnectionTimeout(ui->comboBoxConnectionTimeout->currentData().toInt());
    Setting::saveSocketTimeout(ui->comboBoxSocketTimeout->currentData().toInt());
    Setting::saveACSETimeout(ui->comboBoxACSETimeout->currentData().toInt());
    Setting::saveDIMSETimeout(ui->comboBoxDIMSETimeout->currentData().toInt());
    Setting::saveMaxSendPDU(ui->comboBoxMaxSendPDU->currentData().toInt());
    Setting::saveMaxReceivePDU(ui->comboBoxMaxReceivePDU->currentData().toInt());
}

void MainWindow::addPath(const QString &path)
{
    m_paths.push_front(QDir::toNativeSeparators(path));
    m_paths.removeDuplicates();

    while (m_paths.size() > MAX_PATH)
    {
        m_paths.removeLast();
    }

    ui->comboBoxPaths->clear();
    ui->comboBoxPaths->addItems(m_paths);
    ui->comboBoxPaths->setCurrentIndex(0);
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
    ui->comboBoxSocketTimeout->setEnabled(isDcmtk ? true : false);
    ui->comboBoxACSETimeout->setEnabled(isDcmtk ? true : false);
    ui->comboBoxDIMSETimeout->setEnabled(isDcmtk ? true : false);
    ui->comboBoxMaxSendPDU->setEnabled(isDcmtk ? true : false);
    ui->comboBoxMaxReceivePDU->setEnabled(isDcmtk ? true : false);
}

bool MainWindow::validInput(CheckInput type)
{
    bool valid = true;

    switch (type) {
    case CheckInput::path:
        if (ui->comboBoxPaths->currentText().isEmpty())
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
    if (m_files == nullptr || m_files->isEmpty())
    {
        ui->buttonSend->setEnabled(true);
        return;
    }

    auto store = new StoreThread(ui->radioDcmtk->isChecked() ? Library::dcmtk : Library::gdcm, this);
    store->object()->setFiles(m_files);
    store->object()->setLocalAE(ui->editLocalAE->text().trimmed());
    store->object()->setTargetAE(ui->editTargetAE->text().trimmed());
    store->object()->setHost(ui->editHost->text().trimmed());
    store->object()->setPort(ui->spinBoxPort->value());
    store->object()->setStopWhenError(ui->checkBoxStopWhenError->isChecked());
    store->object()->setConnectionTimeout(ui->comboBoxConnectionTimeout->currentData().toInt());
    if (ui->radioDcmtk->isChecked())
    {
        store->object()->setSocketTimeout(ui->comboBoxSocketTimeout->currentData().toInt());
        store->object()->setACSETimeout(ui->comboBoxACSETimeout->currentData().toInt());
        store->object()->setDIMSETimeout(ui->comboBoxDIMSETimeout->currentData().toInt());
        store->object()->setMaxSendPDU(ui->comboBoxMaxSendPDU->currentData().toInt() * 1024);
        store->object()->setMaxReceivePDU(ui->comboBoxMaxReceivePDU->currentData().toInt() * 1024);
    }

    auto fnResult = [&](const bool result, const QString &log)
    {
        if (!log.isEmpty())
        {
            addColorLog(result, " " + log);
        }
    };

    auto fnProgress = [&](const int percent)
    {
        qDebug() << "Progress:" << percent;
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
        delete m_files;
        m_files = nullptr;
        logDone();
    };

    connect(store, &StoreThread::result, this, fnResult);
    connect(store, &StoreThread::progress, this, fnProgress);
    connect(store, &StoreThread::processed, this, fnProcessed);
    connect(store, &StoreThread::finished, this, fnFinished);
    connect(store, &StoreThread::finished, store, &QObject::deleteLater);
    store->start();
}

void MainWindow::onBrowse()
{
    QString lastPath = (m_paths.empty() ? QDir::currentPath() : ui->comboBoxPaths->currentText());
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
    if (ui->radioDcmtk->isChecked())
    {
        echo->object()->setSocketTimeout(ui->comboBoxSocketTimeout->currentData().toInt());
        echo->object()->setACSETimeout(ui->comboBoxACSETimeout->currentData().toInt());
        echo->object()->setDIMSETimeout(ui->comboBoxDIMSETimeout->currentData().toInt());
        echo->object()->setMaxReceivePDU(ui->comboBoxMaxReceivePDU->currentData().toInt() * 1024);
    }

    auto fnResult = [&](const bool result, const QString &log)
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

    connect(echo, &EchoThread::result, this, fnResult);
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
    scan->object()->setScanDir(ui->comboBoxPaths->currentText());

    auto fnResult = [&](const QStringList *files)
    {
        m_files = files;
        int count = 0;
        if (m_files != nullptr)
        {
            count = m_files->count();
        }
        qDebug() << "File(s): " << count;
        addColorLog(true, QString(" Found %1 file(s)").arg(count));
    };

    auto fnStarted = [&]()
    {
        addLog("Scanning...");
        ui->buttonSend->setEnabled(false);
    };

    auto fnFinished = [&]()
    {
        logDone();
        send();
    };

    connect(scan, &ScanThread::result, this, fnResult);
    connect(scan, &ScanThread::started, this, fnStarted);
    connect(scan, &ScanThread::finished, this, fnFinished);
    connect(scan, &ScanThread::finished, scan, &QObject::deleteLater);
    scan->start();
}
