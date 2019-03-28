#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

class LogModel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(LogModel* model, QWidget* parent = nullptr);

private:
    LogModel* mModel;
};

#endif // MAINWINDOW_H
