#pragma once

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::MainWindow *ui;
    QStringList m_paths;
    QStringList m_sendFiles;

    enum class CheckInput { path, localAE, targetAE, host };

    void loadSetting();
    void saveSetting();
    void addPath(const QString &path);
    void addLog(const QString &log);
    void addColorLog(const bool result, const QString &log);
    void logDone();
    void updateOtherSetting(const bool isDcmtk);
    bool validInput(CheckInput type);
    void send();

private slots:
    void onBrowse();
    void onDcmtk();
    void onGdcm();
    void onEcho();
    void onSend();
};
