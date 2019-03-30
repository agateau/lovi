#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

#include <memory>

class LogFormatLoader;
class LogModel;

class QTreeView;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(LogModel* model, QWidget* parent = nullptr);
    ~MainWindow();

    void loadLogFormat(const QString& fileName);

private:
    void createUi();
    void createActions();
    void onRowsInserted();
    void reloadLogFormat();

    const std::unique_ptr<LogFormatLoader> mLogFormatLoader;
    QToolBar* mToolBar = nullptr;
    QAction* mAutoScrollAction = nullptr;
    LogModel* mModel = nullptr;
    QTreeView* mTreeView = nullptr;
};

#endif // MAINWINDOW_H
