#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

#include <memory>

class Config;
class FileWatcher;
class LogModel;

class QTreeView;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(LogModel* model, QWidget* parent = nullptr);
    ~MainWindow();

    void loadConfig(const QString& fileName);

private:
    void createUi();
    void createActions();
    void onRowsInserted();
    void reloadConfig();

    FileWatcher* const mConfigWatcher;
    QToolBar* mToolBar = nullptr;
    QAction* mAutoScrollAction = nullptr;
    LogModel* mModel = nullptr;
    QTreeView* mTreeView = nullptr;

    std::unique_ptr<Config> mConfig;
};

#endif // MAINWINDOW_H
