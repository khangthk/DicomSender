#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBrowse();
    void onDcmtk(bool checked);
    void onGdcm(bool checked);
    void onEcho();
    void onSend();
private:
    Ui::MainWindow *ui;
    QStringList m_paths;

    void loadSetting();
    void saveSetting();
};
