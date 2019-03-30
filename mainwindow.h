#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

#include <memory>

class LogFormat;
class FileWatcher;
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

    FileWatcher* const mLogFormatWatcher;
    QToolBar* mToolBar = nullptr;
    QAction* mAutoScrollAction = nullptr;
    LogModel* mModel = nullptr;
    QTreeView* mTreeView = nullptr;

    std::unique_ptr<LogFormat> mLogFormat;
};

#endif // MAINWINDOW_H
