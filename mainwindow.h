#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class QProcess;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void launch();
    void log(QString);

private:
    Ui::MainWindow *ui;
    QMap<int, QString> processesAtStart;
    QMap<int, QString> getProcessesList();
    bool TerminateProcessById(int dwProcessId, int uExitCode);
    QStringList filesList;
    QStringList getFilesListToLaunch();
    int currentFile;
    QProcess *process;
};

#endif // MAINWINDOW_H
