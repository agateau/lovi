#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

class LogModel;

class QTreeView;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(LogModel* model, QWidget* parent = nullptr);

private:
    void createUi();
    void createActions();
    void onRowsInserted();

    QToolBar* mToolBar = nullptr;
    QAction* mAutoScrollAction = nullptr;
    LogModel* mModel = nullptr;
    QTreeView* mTreeView = nullptr;
};

#endif // MAINWINDOW_H
