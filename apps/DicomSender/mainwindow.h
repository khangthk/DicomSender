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
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

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
    void addPath(const QString &path);
};
